/*
* 免责声明：
* 本软件是开源程序，您可以自由使用、修改和发布它，使用 GNU 通用公共许可证版本 2（GPLv2）的条款。
* 请注意，在根据GPLv2发布本软件之前，您必须遵守以下条款：
* 1. 任何修改和派生作品必须同样采用GPLv2，并在您的派生作品中提供GPLv2的完整文本。
* 2. 您必须保留原始代码的版权通告，并在您的项目中明确指明原始作者和软件的来源。
* 3. 如果您分发本软件，您必须提供完整的源代码，以满足GPLv2的要求。这包括您的修改和派生作品的源代码。
* 4. 本软件是按"原样"提供的，不附带任何明示或暗示的保证。作者对于使用本软件造成的任何直接、间接、偶发、特殊、典型或惩戒性损害不承担责任。
* 使用此代码即表示您同意遵守GPLv2的所有要求和免责声明的条件。
* 请仔细阅读GNU通用公共许可证版本2以了解更多详细信息。您可以在 http ://www.gnu.org/licenses/gpl-2.0.html 上找到GPLv2的完整文本。
* [作者姓名]: bilibili：随风而息
* [地址]:https://github.com/AMXZzzz/SF_TRT_62.git
* [日期]: 2023/10/26
*/

#include <fstream>
#include "frame_trt.h"

#define PERFORMANCE 0		//! 开启高性能
#define EXECUTE	0			//! 使用同步

#define MESSAGEBOXA(x,y) MessageBoxA(NULL, x, "TensorRt Error", y)	// 0x00000000L is MB_OK
#define asserthr(x) if(!x) return false;
#define tostr(x) std::to_string(x).c_str()

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
#define LOGINFO(format,...) if (m_logger) {m_logger->info(TRTHEAD ## format, __VA_ARGS__);} else {std::cout<<"spdlog未初始化"<<std::endl;}
#define LOGWARN(format,...)  if (m_logger) {m_logger->warn(TRTHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlog未初始化"<<std::endl;}
#define LOGERROR(format,...) if (m_logger) {m_logger->error(TRTHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlog未初始化"<<std::endl;}
#define CHECK_TRT(x) (x==NULL)

// -------------------------------------------------------- 
static std::wstring UTF8ToUnicode(const char* strSrc) {
	std::wstring wstrRet;
	if (NULL != strSrc) {
		int len = MultiByteToWideChar(CP_UTF8, 0, strSrc, -1, NULL, 0) * sizeof(WCHAR);
		WCHAR* strDst = new(std::nothrow) WCHAR[len + 1];
		if (NULL != strDst) {
			MultiByteToWideChar(CP_UTF8, 0, strSrc, -1, strDst, len);
			wstrRet = strDst;;
			delete[]strDst;
		}
	}
	return wstrRet;
}

static std::string UnicodeToAnsi(const WCHAR* strSrc) {
	std::string strRet;

	if (NULL != strSrc) {
		int len = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
		char* strDst = new(std::nothrow) char[len + 1];
		if (NULL != strDst) {
			WideCharToMultiByte(CP_ACP, 0, strSrc, -1, strDst, len, NULL, NULL);
			strRet = strDst;
			delete[]strDst;
		}
	}
	return strRet;
}

static std::string UTF8ToAnsi(const char* strSrc) {
	return UnicodeToAnsi(UTF8ToUnicode(strSrc).c_str());
}

static inline unsigned int getElementSize(nvinfer1::DataType type) {
	switch (type) {
	case nvinfer1::DataType::kINT32: return sizeof(INT32);
	case nvinfer1::DataType::kFLOAT: return sizeof(float);
	case nvinfer1::DataType::kHALF: return sizeof(float) / 2;		// float的一半
	case nvinfer1::DataType::kINT8: return sizeof(float) / 4;		// half的一半
	default: return 4;	// 未知类型统一为4
	}
}

static int64_t volume(const nvinfer1::Dims& dims) {
	int64_t size = 1;
	for (int i = 0; i < dims.nbDims; ++i)
		size *= dims.d[i];
	return size;
}

// ------------------------------------------------------------------------------
bool ITensorRt::CheckPrt() {
	if (m_yolo == nullptr) {
		std::cout << "yolo对象为空" << std::endl;
		return false;
	}
	if (m_logger == nullptr) {
		std::cout << "spdlog对象为空" << std::endl;
		return false;
	}
	return true;
}

bool ITensorRt::AnalyticalModel(const char* engine_path) {
	//! 检查传入DML对象的指针是否有效
	if (!CheckPrt()) {
		return false;
	}
	//! 解析模型
	return InitInterface(engine_path);
}

bool ITensorRt::AnalyticalModel(const std::string engine_path) {
	//! 检查传入DML对象的指针是否有效
	if (!CheckPrt()) {
		return false;
	}
	//! 解析模型
	return InitInterface(engine_path.data());
}

IStates ITensorRt::getLastErrorInfo() {
	if (errors.empty()) {
		return IStates();
	}
	else {
		int idx = errors.size() - 1;
		return IStates(errors[idx].is_success(), errors[idx].getErrorMessage());
	}
}

void ITensorRt::markError(std::string info, bool error = false) {
	MESSAGEBOXA(info.data(), MB_OK);
	errors.push_back(IStates(error, std::string(info)));
}

bool ITensorRt::loadSerializedFile(const char* engine_path) {

	std::ifstream file(UTF8ToAnsi(engine_path).data(), std::ios::binary);
	if (!file.good()) {
		LOGWARN("读取engine错误,请检查文件和路径是否正确,小概率为无读取权限");
		markError("读取engine错误,请检查文件和路径是否正确,小概率为无读取权限");
		return false;
	}
	file.seekg(0, file.end);
	char_stream_size = file.tellg();
	if (char_stream_size == 0) {
		LOGWARN("读取engine数据错误,请检查engine文件大小");
		markError("读取engine数据错误,请检查engine文件大小");
		return false;
	}

	file.seekg(0, file.beg);
	char_stream = new char[char_stream_size];
	assert(char_stream);
	file.read(char_stream, char_stream_size);
	file.close();
	return true;
}

bool ITensorRt::InitInterface(const char* engine_path) {
	// 设置运行设备
	cudaSetDevice(m_equipment);

	// 加载engine文件
	if (!loadSerializedFile(engine_path)) {
		return false;
	}

	// 初始化接口
	m_runtime = nvinfer1::createInferRuntime(m_nvlog);
	if (CHECK_TRT(m_runtime)) {
		LOGWARN("创建 nvinfer1 Runtime失败");
		markError("创建 nvinfer1 Runtime失败");
		return false;
	}

	//! 加载engine并逆序列化
	m_engine = m_runtime->deserializeCudaEngine(char_stream, char_stream_size);
	if (CHECK_TRT(m_engine)) {
		LOGWARN("创建Engine失败,请重新生成engine文件");
		markError("创建Engine失败,请重新生成engine文件");
		return false;
	}
	//! 创建上下文
	m_context = m_engine->createExecutionContext();
	if (CHECK_TRT(m_context)) {
		LOGWARN("创建 context 失败,未知原因");
		markError("创建 context 失败,未知原因");
		return false;
	}

	//! 释放字节流
	if (char_stream != nullptr) {
		delete[] char_stream;
		char_stream = nullptr;
	}

	//! 创建cuda流
	CUDA_CHECK(cudaStreamCreate(&m_stream));
	LOGINFO("cuda接口初始化 Done...");

	// 解析模型
	if (!parseModelInfo()) {
		//! 解析模型 EX
		//! 即通过节点名注册失败,可能是层名是自定义的
		//! 尝试通过维度注册
		if (!parseModelInfoEx())  return false;

		LOGINFO("通过parseModelInfoEx 注册节点");
	}
	else {
		LOGINFO("parseModelInfo注册节点 Done...");
	}

	// 申请host内存
	input_memory = new float[input_memory_size];
	output_memory = new float[output_memory_size];

	LOGINFO("申请host内存 Done...");
	return true;
}

bool ITensorRt::parseModelInfo() {

	// 获取指定的层索引
	input_idx = m_engine->getBindingIndex(*m_yolo->getInputName());
	output_idx = m_engine->getBindingIndex(*m_yolo->getOutputName());

	// 检查
	if (input_idx == -1) {
		LOGWARN("找不到名为{}的输入节点", *m_yolo->getInputName());
		markError("找不到输入节点");
		return false;
	}

	if (output_idx == -1) {
		LOGWARN("找不到名为{}的输出节点", *m_yolo->getOutputName());
		markError("找不到输出节点");
		return false;
	}

	gpu_buffers.resize(m_engine->getNbBindings());
	// 注册节点
	for (size_t i = 0; i < m_engine->getNbBindings(); i++) {
		// 获取该层的输入shape和类型
		nvinfer1::Dims dims = m_engine->getBindingDimensions(i);
		nvinfer1::DataType type = m_engine->getBindingDataType(i);
		if (i == input_idx) {
			asserthr(registerInput(dims, type));
		}
		else if (i == output_idx) {
			asserthr(registerOutput(dims, type));
		}
		else {
			// 可能存在其他不使用的节点,但也要注册,且内存也要对位
			CUDA_CHECK(cudaMalloc(&gpu_buffers[i], volume(dims) * getElementSize(type)));
		}
	}
	return true;
}

bool ITensorRt::parseModelInfoEx() {
	bool in = false;
	bool out = false;
	for (size_t i = 0; i < m_engine->getNbBindings(); i++) {
		nvinfer1::Dims dims = m_engine->getBindingDimensions(i);
		nvinfer1::DataType type = m_engine->getBindingDataType(i);
		if (dims.nbDims == 4 && dims.d[1] == 3 && dims.d[2] == dims.d[3]) {
			input_idx = i;
			asserthr(registerInput(dims, type));
			in = true;
		}
		else if (dims.nbDims == 3 && dims.d[0] == 1 && (dims.d[1] != dims.d[2])) {
			output_idx = i;
			asserthr(registerOutput(dims, type));
			out = true;
		}
		else {
			CUDA_CHECK(cudaMalloc(&gpu_buffers[i], volume(dims) * getElementSize(type)));
		}
	}
	// 通过维度找到
	if (in && out) {
		return true;
	}

	// 未找到
	if (!in) {
		LOGWARN("通过维度解析模型失败,输入未找到");
	}
	if (!out) {
		LOGWARN("通过维度解析模型失败,输出未找到");
	}
	return false;
}

bool ITensorRt::registerInput(nvinfer1::Dims& dims, nvinfer1::DataType& type) {
	// 检查输入对称
	if (dims.d[2] != dims.d[3]) {
		LOGWARN("不支持输入(image size)不对称的模型");
		markError("不支持输入(image size)不对称的模型");
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

	//! 打印
	for (size_t i = 0; i < temp.size(); i++) {
		std::cout << TRTHEAD << "input维度:" << temp.at(i) << std::endl;
	}

	// 设置输入容器
	m_yolo->setInputDims(temp);

	// 获取输入内存大小, 内存和显存对位相等
	input_memory_size = volume(dims) * getElementSize(type);

	// 注册输入节点
	CUDA_CHECK(cudaMalloc(&gpu_buffers[input_idx], input_memory_size));

	// 输出到日志
	LOGINFO("输入Shape: {} {} {} {}", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]), tostr(dims.d[3]));
	return true;
}

bool ITensorRt::registerOutput(nvinfer1::Dims& dims, nvinfer1::DataType& type) {
	if (m_yolo->dims_error(dims.d[1], dims.d[2])) {
		LOGWARN("输出shape和框架不匹配.Shape is [ {} {} {} ] ", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]));
		markError("模型和框架不匹配");
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
	//! 打印
	for (size_t i = 0; i < temp.size(); i++) {
		std::cout << TRTHEAD << "output维度:" << temp.at(i) << std::endl;
	}
	// 设置输出容器
	m_yolo->setOutputDims(temp);

	// 获取输出注册内存大小
	output_memory_size = volume(dims) * getElementSize(type);

	// 注册输出节点
	CUDA_CHECK(cudaMalloc(&gpu_buffers[output_idx], output_memory_size));

	// 输出到日志
	LOGINFO("输出Shape: {} {} {}", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]));
	return true;
}

void ITensorRt::imageToBlob(cv::Mat& img) {
	// 改版
	int img_rows = img.rows;
	int img_clos = img.cols;
	for (size_t h = 0; h < img_rows; ++h) {
		const cv::Vec3b* p1 = img.ptr<cv::Vec3b>(h);
		for (size_t w = 0; w < img_clos; ++w) {
			for (size_t i = 0; i < img.channels(); ++i) {
				input_memory[i * img_rows * img_rows + h * img_clos + w] = (p1[w][static_cast<size_t>(3) - 1 - i]) * m_yolo->getNormalized();
			}
		}
	}
}

void ITensorRt::InferExecute() {
	// 同步推理
#if PERFORMANCE
	cudaMemcpyAsync(gpu_buffers[input_idx], input_memory, input_memory_size, cudaMemcpyHostToDevice, m_stream);
	m_context->executeV2(gpu_buffers);
	cudaMemcpyAsync(output_memory, gpu_buffers[output_idx], output_memory_size, cudaMemcpyDeviceToHost, m_stream);
	cudaStreamSynchronize(m_stream);
#else
	CUDA_CHECK2(cudaMemcpyAsync(gpu_buffers[input_idx], input_memory, input_memory_size, cudaMemcpyHostToDevice, m_stream));
	m_context->executeV2(gpu_buffers.data());
	CUDA_CHECK2(cudaMemcpyAsync(output_memory, gpu_buffers[output_idx], output_memory_size, cudaMemcpyDeviceToHost, m_stream));
	//cudaStreamSynchronize(m_stream);
#endif
}

void ITensorRt::InferEnqueue() {
	// 异步推理
#if PERFORMANCE
	(cudaMemcpyAsync(gpu_buffers[input_idx], input_memory, input_memory_size, cudaMemcpyHostToDevice, m_stream));
	m_context->enqueueV2(gpu_buffers, m_stream, NULL);
	(cudaMemcpyAsync(output_memory, gpu_buffers[output_idx], output_memory_size, cudaMemcpyDeviceToHost, m_stream));
	cudaStreamSynchronize(m_stream);
#else
	CUDA_CHECK2(cudaMemcpyAsync(gpu_buffers[input_idx], input_memory, input_memory_size, cudaMemcpyHostToDevice, m_stream));
	m_context->enqueueV2(gpu_buffers.data(), m_stream, NULL);
	CUDA_CHECK2(cudaMemcpyAsync(output_memory, gpu_buffers[output_idx], output_memory_size, cudaMemcpyDeviceToHost, m_stream));
	cudaStreamSynchronize(m_stream);
#endif
}

void ITensorRt::Detect(cv::Mat& img) {
	imageToBlob(img);
#if EXECUTE
	InferExecute();	//! 同步推理
#else
	InferEnqueue();	//! 异步推理
#endif
	m_yolo->DecodeOutput(output_memory);
}

void ITensorRt::Release() {
	if (m_stream) cudaStreamDestroy(m_stream);
	for (int i = 0; i < m_engine->getNbBindings(); i++) {   //循环释放
		if (gpu_buffers.at(i)) {
			CUDA_CHECK2(cudaFree(gpu_buffers.at(i)));
		}
	}
	if (m_context) m_context->destroy();
	if (m_engine) m_engine->destroy();
	if (m_runtime) m_runtime->destroy();
	if (char_stream != nullptr) {
		delete[] char_stream;
		char_stream = nullptr;
	}
	if (input_memory != nullptr) {
		delete[] input_memory;
		input_memory = nullptr;
	}
	if (output_memory != nullptr) {
		delete[] output_memory;
		output_memory = nullptr;
	}
	LOGINFO("释放TensorRt资源 Done \n\n\n\n");
	std::cout << "TensorRt对象释放" << std::endl;
	delete this;
}

