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

// ������
#define TRTHEAD "[TRT]: "
#define LOGINFO(format,...) if (_logger) {_logger->info(TRTHEAD ## format, __VA_ARGS__);} 
#define LOGWARN(format,...)  if (_logger) {_logger->warn(TRTHEAD ## format, __VA_ARGS__);}
#define LOGERROR(format,...) if (_logger) {_logger->error(TRTHEAD ## format, __VA_ARGS__);}

#define CHECK_TRT(x) (x==NULL)

static inline unsigned int getElementSize(nvinfer1::DataType type) {
	switch (type) {
	case nvinfer1::DataType::kINT32: return sizeof(INT32);
	case nvinfer1::DataType::kFLOAT: return sizeof(float);
	case nvinfer1::DataType::kHALF: return sizeof(float) / 2;		// float��һ��
	case nvinfer1::DataType::kINT8: return sizeof(float) / 4;		// half��һ��
	default: 4;	// δ֪����ͳһΪ4
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
		LOGWARN("��ȡengine����,�����·��,С����Ϊ�޶�ȡȨ��");
		markError("��ȡengine����,�����·��,С����Ϊ�޶�ȡȨ��", State::TRT_LoadFlie);
		return false;
	}
	file.seekg(0, file.end);
	char_stream_size = file.tellg();
	if (char_stream_size == 0) {
		LOGWARN("��ȡengine���ݴ���,����engine�ļ���С");
		markError("��ȡengine���ݴ���,����engine�ļ���С", State::TRT_LoadDate);
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
	// ���������豸
	cudaSetDevice(_equipment);

	// ����engine�ļ�
	asserthr(loadSerializedFile(engine_path));
	
	// ��ʼ���ӿ�
	_runtime = nvinfer1::createInferRuntime(_nvlog);
	if (CHECK_TRT(_runtime)) {
		LOGWARN("���� nvinfer1 Runtimeʧ��");
		markError("���� nvinfer1 Runtimeʧ��",State::TRT_Runtime);
		return false;
	}

	_engine = _runtime->deserializeCudaEngine(char_stream, char_stream_size);
	if (CHECK_TRT(_engine)) {
		LOGWARN("����Engineʧ��,����������engine�ļ�");
		markError("����Engineʧ��,����������engine�ļ�", State::TRT_Engien);
		return false;
	}

	_context = _engine->createExecutionContext();
	if (CHECK_TRT(_context)) {
		LOGWARN("���� context ʧ��,δ֪ԭ��");
		markError("���� context ʧ��,δ֪ԭ��", State::TRT_Context);
		return false;
	}
	delete[] char_stream;
	CUDA_CHECK(cudaStreamCreate(&_stream));
	LOGINFO("cuda�ӿڳ�ʼ�� Done...");

	// ����ģ��
	if (!parseModelInfo()) {
		//! ����ģ��EX
		//! ��ͨ���ڵ���ע��ʧ��,�����ǲ������Զ����
		//! ����ͨ��ά��ע��
		if (!parseModelInfoEx())  return false;
		
		LOGINFO("ͨ��parseModelInfoEx ע��ڵ�");
	} else {
		LOGINFO("parseModelInfoע��ڵ� Done...");
	}

	// ����host�ڴ�
	input_memory = new float[input_memory_size];
	output_memory = new float[output_memory_size];

	LOGINFO("����host�ڴ� Done...");
	return true;
}

bool sf::trt::ITensorRt::parseModelInfo() {

	// ��ȡ������
	input_idx = _engine->getBindingIndex(*_yolo->getInputName());
	output_idx = _engine->getBindingIndex(*_yolo->getOutputName());

	// ���
	if (input_idx == -1) {
		LOGWARN("�Ҳ�����Ϊ{}������ڵ�", *_yolo->getInputName());
		markError("�Ҳ�������ڵ�", State::UNKONEERR);
		return false;
	}

	if (output_idx == -1) {
		LOGWARN("�Ҳ�����Ϊ{}������ڵ�", *_yolo->getOutputName());
		markError("�Ҳ�������ڵ�", State::UNKONEERR);
		return false;
	}

	gpu_buffers.resize(_engine->getNbBindings());
	// ע��ڵ�
	for (size_t i = 0; i < _engine->getNbBindings(); i++) {
		// ��ȡ�ò������shape������
		nvinfer1::Dims dims = _engine->getBindingDimensions(i);
		nvinfer1::DataType type = _engine->getBindingDataType(i);
		if (i == input_idx) {
			asserthr(registerInput(dims, type));
		} else if (i == output_idx) {
			asserthr(registerOutput(dims, type));
		} else {
			// ���ܴ���������ʹ�õĽڵ�,��ҲҪע��,���ڴ�ҲҪ��λ
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
	// ͨ��ά���ҵ�
	if (in && out) {
		return true;
	}

	// δ�ҵ�
	if (!in) {
		LOGWARN("ͨ��ά�Ƚ���ģ��,����δ�ҵ�");
	}
	if (!out) {
		LOGWARN("ͨ��ά�Ƚ���ģ��,���δ�ҵ�");
	}
	return false;
}

bool sf::trt::ITensorRt::registerInput(nvinfer1::Dims& dims, nvinfer1::DataType& type) {
	// �������Գ�
	if (dims.d[2]!= dims.d[3]) {
		LOGWARN("��֧������(image size)���ԳƵ�ģ��");
		markError("��֧������(image size)���ԳƵ�ģ��", State::DimsNotEqual);
		return false;
	}

	// ����ά��
	std::vector<int64_t> temp;
	for (size_t i = 0; i < dims.nbDims; i++) 
		temp.push_back(dims.d[i]);

	if (temp.empty()) {
		LOGWARN("��ȡ��������(shape)ʧ��");
		return false;
	}

	// ������������
	_yolo->setInputDims(temp);

	// ��ȡ�����ڴ��С, �ڴ���Դ��λ���
	input_memory_size = volume(dims) * getElementSize(type);

	// ע������ڵ�
	CUDA_CHECK(cudaMalloc(&gpu_buffers[input_idx], input_memory_size));

	// �������־
	LOGINFO("����Shape: {} {} {} {}", tostr(dims.d[0]),tostr(dims.d[1]), tostr(dims.d[2]), tostr(dims.d[3]));
	return true;
}

bool sf::trt::ITensorRt::registerOutput(nvinfer1::Dims& dims, nvinfer1::DataType& type) {
	if (_yolo->dims_error(dims.d[1], dims.d[2])) {
		LOGWARN("���shape�Ϳ�ܲ�ƥ��.Shape is [ {} {} {} ] ", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]));
		markError("ģ�ͺͿ�ܲ�ƥ��", State::FrameNotMatch);
		return false;
	}

	// ����ά��
	std::vector<int64_t> temp;
	for (size_t i = 0; i < dims.nbDims; i++)
		temp.push_back(dims.d[i]);

	if (temp.empty()) {
		LOGWARN("��ȡ�������(shape)ʧ��");
		return false;
	}

	// �����������
	_yolo->setOutputDims(temp);

	// ��ȡ���ע���ڴ��С
	output_memory_size = volume(dims) * getElementSize(type);

	// ע������ڵ�
	CUDA_CHECK(cudaMalloc(&gpu_buffers[output_idx], output_memory_size));

	// �������־
	LOGINFO("���Shape: {} {} {}", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]));
	return true;
}

void sf::trt::ITensorRt::imageToBlob(cv::Mat& img) {
	for (int c = 0; c < 3; ++c) { 
		for (int h = 0; h < img.rows; ++h) {
			//��ȡ��i��������ָ�� 
			cv::Vec3b* p1 = img.ptr<cv::Vec3b>(h);
			for (int w = 0; w < img.cols; ++w) {
				input_memory[c * img.cols * img.rows + h * img.cols + w] = (p1[w][c]) *  _yolo->getNormalized();   //yolov5

			}
		}
	}
}

#define PERFORMANCE 0	
void sf::trt::ITensorRt::InferExecute() {
	// ͬ������
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
	// �첽����
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
	if (!_yolo) 	return IStates(State::UNKONEERR, "yolo���ñ����");
	if (!_logger) 	return IStates(State::UNKONEERR, "��־����");
	return IStates();
}