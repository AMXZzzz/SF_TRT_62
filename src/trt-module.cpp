#include <fstream>
#include "trt-module.h"

#ifndef CUDA_CHECK
#define CUDA_CHECK(callstr)\
        if (callstr != cudaSuccess) {\
            std::cerr << "CUDA error " << callstr << " at " << __FILE__ << ":" << __LINE__;\
			LOGWARN("CUDA error:{} in {}", __FILE__,tostr(__LINE__));\
			return false;\
        }

#endif  // CUDA_CHECK

#ifndef CUDA_CHECK2
#define CUDA_CHECK2(callstr)\
        if (callstr != cudaSuccess) {\
            std::cerr << "CUDA error " << callstr << " at " << __FILE__ << ":" << __LINE__<< std::endl;\
			LOGWARN("CUDA error:{} in {}", __FILE__,tostr(__LINE__));\
        }
		
#endif  //       

// 宏重载
#define TRTHEAD "[TRT]: "
#define LOGINFO(format,...) if (_logger) {_logger->info(TRTHEAD ## format, __VA_ARGS__);} 
#define LOGWARN(format,...)  if (_logger) {_logger->warn(TRTHEAD ## format, __VA_ARGS__);}
#define LOGERROR(format,...) if (_logger) {_logger->error(TRTHEAD ## format, __VA_ARGS__);}

#define CHECK_TRT(x) (x==NULL)

static inline unsigned int getElementSize(nvinfer1::DataType type) {
	switch (type) {
	case nvinfer1::DataType::kINT32: return sizeof(INT32);
	case nvinfer1::DataType::kFLOAT: return sizeof(float);
	case nvinfer1::DataType::kHALF: return sizeof(float) / 2;		// float的一半
	case nvinfer1::DataType::kINT8: return sizeof(float) / 4;		// half的一半
	default: 4;	// 未知类型统一为4
	}
}

static int64_t volume(const nvinfer1::Dims& dims) {
	int64_t size = 1;
	for (int i = 0; i < dims.nbDims; ++i)
		size *= dims.d[i];
	return size;
}

bool sf::trt::ITensorRt::AnalyticalModel(const char* engine_path) {
	return InitInterface(engine_path);
}
bool sf::trt::ITensorRt::AnalyticalModel(const std::string engine_path) {
	return InitInterface(engine_path.data());
}

IStates sf::trt::ITensorRt::getLastErrorInfo() {
	if (_trtstates.empty()) {
		return IStates();
	} else {
		int idx = _trtstates.size() - 1;
		return IStates(_trtstates[idx].err_code(), _trtstates[idx].msg());
	}
}

void sf::trt::ITensorRt::Release() {
	if (!char_stream) delete[] char_stream;
	delete this;
}

void sf::trt::ITensorRt::markError(std::string info, State sta) {
	MESSAGEBOXA(info.data(), MB_OK);
	_trtstates.push_back(IStates(sta, std::string(info)));
}

bool sf::trt::ITensorRt::loadSerializedFile(const char* engine_path) {

	std::ifstream file(UTF8ToAnsi(engine_path).data(), std::ios::binary);
	if (!file.good()) {
		LOGWARN("读取engine错误,请检查和路径,小概率为无读取权限");
		markError("读取engine错误,请检查和路径,小概率为无读取权限", State::TRT_LoadFlie);
		return false;
	}
	file.seekg(0, file.end);
	char_stream_size = file.tellg();
	if (char_stream_size == 0) {
		LOGWARN("读取engine数据错误,请检查engine文件大小");
		markError("读取engine数据错误,请检查engine文件大小", State::TRT_LoadDate);
		return false;
	}

	file.seekg(0, file.beg);
	char_stream = new char[char_stream_size];
	assert(char_stream);
	file.read(char_stream, char_stream_size);
	file.close();
	return true;
}

bool sf::trt::ITensorRt::InitInterface(const char* engine_path) {
	// 设置运行设备
	cudaSetDevice(_equipment);

	// 加载engine文件
	asserthr(loadSerializedFile(engine_path));
	
	// 初始化接口
	_runtime = nvinfer1::createInferRuntime(_nvlog);
	if (CHECK_TRT(_runtime)) {
		LOGWARN("创建 nvinfer1 Runtime失败");
		markError("创建 nvinfer1 Runtime失败",State::TRT_Runtime);
		return false;
	}

	_engine = _runtime->deserializeCudaEngine(char_stream, char_stream_size);
	if (CHECK_TRT(_engine)) {
		LOGWARN("创建Engine失败,请重新生成engine文件");
		markError("创建Engine失败,请重新生成engine文件", State::TRT_Engien);
		return false;
	}

	_context = _engine->createExecutionContext();
	if (CHECK_TRT(_context)) {
		LOGWARN("创建 context 失败,未知原因");
		markError("创建 context 失败,未知原因", State::TRT_Context);
		return false;
	}
	delete[] char_stream;
	CUDA_CHECK(cudaStreamCreate(&_stream));
	LOGINFO("cuda接口初始化 Done...");

	// 解析模型
	if (!parseModelInfo()) {
		//! 解析模型EX
		//! 即通过节点名注册失败,可能是层名是自定义的
		//! 尝试通过维度注册
		if (!parseModelInfoEx())  return false;
		
		LOGINFO("通过parseModelInfoEx 注册节点");
	} else {
		LOGINFO("parseModelInfo注册节点 Done...");
	}

	// 申请host内存
	input_memory = new float[input_memory_size];
	output_memory = new float[output_memory_size];

	LOGINFO("申请host内存 Done...");
	return true;
}

bool sf::trt::ITensorRt::parseModelInfo() {

	// 获取绑定索引
	input_idx = _engine->getBindingIndex(*_yolo->getInputName());
	output_idx = _engine->getBindingIndex(*_yolo->getOutputName());

	// 检查
	if (input_idx == -1) {
		LOGWARN("找不到名为{}的输入节点", *_yolo->getInputName());
		markError("找不到输入节点", State::UNKONEERR);
		return false;
	}

	if (output_idx == -1) {
		LOGWARN("找不到名为{}的输出节点", *_yolo->getOutputName());
		markError("找不到输出节点", State::UNKONEERR);
		return false;
	}

	gpu_buffers.resize(_engine->getNbBindings());
	// 注册节点
	for (size_t i = 0; i < _engine->getNbBindings(); i++) {
		// 获取该层的输入shape和类型
		nvinfer1::Dims dims = _engine->getBindingDimensions(i);
		nvinfer1::DataType type = _engine->getBindingDataType(i);
		if (i == input_idx) {
			asserthr(registerInput(dims, type));
		} else if (i == output_idx) {
			asserthr(registerOutput(dims, type));
		} else {
			// 可能存在其他不使用的节点,但也要注册,且内存也要对位
			CUDA_CHECK(cudaMalloc(&gpu_buffers[i], volume(dims) * getElementSize(type)));	
		}
	}
	return true;
}

bool sf::trt::ITensorRt::parseModelInfoEx() {
	bool in = false;
	bool out = false;
	for (size_t i = 0; i < _engine->getNbBindings(); i++) {
		nvinfer1::Dims dims = _engine->getBindingDimensions(i);
		nvinfer1::DataType type = _engine->getBindingDataType(i);
		if (dims.nbDims == 4 && dims.d[1] == 3 && dims.d[2] == dims.d[3]) {
			input_idx = i;
			asserthr(registerInput(dims, type));
			in = true;
		} else if (dims.nbDims == 3 && dims.d[0] == 1 && (dims.d[1] != dims.d[2])) {
			output_idx = i;
			asserthr(registerOutput(dims, type));
			out = true;
		} else {
			CUDA_CHECK(cudaMalloc(&gpu_buffers[i], volume(dims) * getElementSize(type)));
		}
	}
	// 通过维度找到
	if (in && out) {
		return true;
	}

	// 未找到
	if (!in) {
		LOGWARN("通过维度解析模型,输入未找到");
	}
	if (!out) {
		LOGWARN("通过维度解析模型,输出未找到");
	}
	return false;
}

bool sf::trt::ITensorRt::registerInput(nvinfer1::Dims& dims, nvinfer1::DataType& type) {
	// 检查输入对称
	if (dims.d[2]!= dims.d[3]) {
		LOGWARN("不支持输入(image size)不对称的模型");
		markError("不支持输入(image size)不对称的模型", State::DimsNotEqual);
		return false;
	}

	// 处理维度
	std::vector<int64_t> temp;
	for (size_t i = 0; i < dims.nbDims; i++) 
		temp.push_back(dims.d[i]);

	if (temp.empty()) {
		LOGWARN("获取输入数据(shape)失败");
		return false;
	}

	// 设置输入容器
	_yolo->setInputDims(temp);

	// 获取输入内存大小, 内存和显存对位相等
	input_memory_size = volume(dims) * getElementSize(type);

	// 注册输入节点
	CUDA_CHECK(cudaMalloc(&gpu_buffers[input_idx], input_memory_size));

	// 输出到日志
	LOGINFO("输入Shape: {} {} {} {}", tostr(dims.d[0]),tostr(dims.d[1]), tostr(dims.d[2]), tostr(dims.d[3]));
	return true;
}

bool sf::trt::ITensorRt::registerOutput(nvinfer1::Dims& dims, nvinfer1::DataType& type) {
	if (_yolo->dims_error(dims.d[1], dims.d[2])) {
		LOGWARN("输出shape和框架不匹配.Shape is [ {} {} {} ] ", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]));
		markError("模型和框架不匹配", State::FrameNotMatch);
		return false;
	}

	// 处理维度
	std::vector<int64_t> temp;
	for (size_t i = 0; i < dims.nbDims; i++)
		temp.push_back(dims.d[i]);

	if (temp.empty()) {
		LOGWARN("获取输出数据(shape)失败");
		return false;
	}

	// 设置输出容器
	_yolo->setOutputDims(temp);

	// 获取输出注册内存大小
	output_memory_size = volume(dims) * getElementSize(type);

	// 注册输出节点
	CUDA_CHECK(cudaMalloc(&gpu_buffers[output_idx], output_memory_size));

	// 输出到日志
	LOGINFO("输出Shape: {} {} {}", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]));
	return true;
}

void sf::trt::ITensorRt::imageToBlob(cv::Mat& img) {
	for (int c = 0; c < 3; ++c) { 
		for (int h = 0; h < img.rows; ++h) {
			//获取第i行首像素指针 
			cv::Vec3b* p1 = img.ptr<cv::Vec3b>(h);
			for (int w = 0; w < img.cols; ++w) {
				input_memory[c * img.cols * img.rows + h * img.cols + w] = (p1[w][c]) / 255.0f;   //yolov5
			}
		}
	}
}

#define PERFORMANCE 0	
void sf::trt::ITensorRt::InferExecute() {
	// 同步推理
#if PERFORMANCE
	cudaMemcpyAsync(gpu_buffers[input_idx], input_memory, input_memory_size, cudaMemcpyHostToDevice, _stream);
	_context->executeV2(gpu_buffers);
	cudaMemcpyAsync(output_memory, gpu_buffers[output_idx], output_memory_size, cudaMemcpyDeviceToHost, _stream);
	cudaStreamSynchronize(_stream);
#else

	CUDA_CHECK2(cudaMemcpyAsync(gpu_buffers[input_idx], input_memory, input_memory_size, cudaMemcpyHostToDevice, _stream));
	_context->executeV2(gpu_buffers.data());
	CUDA_CHECK2(cudaMemcpyAsync(output_memory, gpu_buffers[output_idx], output_memory_size, cudaMemcpyDeviceToHost, _stream));
	//cudaStreamSynchronize(_stream);
#endif
}
void sf::trt::ITensorRt::InferEnqueue() {
	// 异步推理
#if PERFORMANCE
	(cudaMemcpyAsync(gpu_buffers[input_idx], input_memory, input_memory_size, cudaMemcpyHostToDevice, _stream));
	_context->enqueueV2(gpu_buffers, _stream, NULL);
	(cudaMemcpyAsync(output_memory, gpu_buffers[output_idx], output_memory_size, cudaMemcpyDeviceToHost, _stream));
	cudaStreamSynchronize(_stream);
#else
	CUDA_CHECK2(cudaMemcpyAsync(gpu_buffers[input_idx], input_memory, input_memory_size, cudaMemcpyHostToDevice, _stream));
	_context->enqueueV2(gpu_buffers.data(), _stream, NULL);
	CUDA_CHECK2(cudaMemcpyAsync(output_memory, gpu_buffers[output_idx], output_memory_size, cudaMemcpyDeviceToHost, _stream));
	cudaStreamSynchronize(_stream);
#endif
}

void sf::trt::ITensorRt::Detect(cv::Mat& img) {
	imageToBlob(img);
	InferEnqueue();
	_yolo->DecodeOutput(output_memory,img);
}

IStates sf::trt::TRTFactory::QueryInterface(void** trt) {
	IStates hr = checkFrameMemberPtr();
	if (hr.is_error()) {
		return hr;
	}
	*trt = new sf::trt::ITensorRt(_yolo, _logger, _equipment);
	return IStates();
}
IStates sf::trt::TRTFactory::checkFrameMemberPtr() {
	if (!_yolo) 	return IStates(State::UNKONEERR, "yolo配置表错误");
	if (!_logger) 	return IStates(State::UNKONEERR, "日志错误");
	return IStates();
}