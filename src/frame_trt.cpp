/*
* ����������
* ������ǿ�Դ��������������ʹ�á��޸ĺͷ�������ʹ�� GNU ͨ�ù������֤�汾 2��GPLv2�������
* ��ע�⣬�ڸ���GPLv2���������֮ǰ�������������������
* 1. �κ��޸ĺ�������Ʒ����ͬ������GPLv2����������������Ʒ���ṩGPLv2�������ı���
* 2. �����뱣��ԭʼ����İ�Ȩͨ�棬����������Ŀ����ȷָ��ԭʼ���ߺ��������Դ��
* 3. ������ַ���������������ṩ������Դ���룬������GPLv2��Ҫ������������޸ĺ�������Ʒ��Դ���롣
* 4. ������ǰ�"ԭ��"�ṩ�ģ��������κ���ʾ��ʾ�ı�֤�����߶���ʹ�ñ������ɵ��κ�ֱ�ӡ���ӡ�ż�������⡢���ͻ�ͽ����𺦲��е����Ρ�
* ʹ�ô˴��뼴��ʾ��ͬ������GPLv2������Ҫ�������������������
* ����ϸ�Ķ�GNUͨ�ù������֤�汾2���˽������ϸ��Ϣ���������� http ://www.gnu.org/licenses/gpl-2.0.html ���ҵ�GPLv2�������ı���
* [��������]: bilibili������Ϣ
* [��ַ]:https://github.com/AMXZzzz/SF_TRT_62.git
* [����]: 2023/10/26
*/

#include <fstream>
#include "frame_trt.h"

#define PERFORMANCE 0		//! ����������
#define EXECUTE	0			//! ʹ��ͬ��

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

// ������
#define TRTHEAD "[TRT]: "
#define LOGINFO(format,...) if (m_logger) {m_logger->info(TRTHEAD ## format, __VA_ARGS__);} else {std::cout<<"spdlogδ��ʼ��"<<std::endl;}
#define LOGWARN(format,...)  if (m_logger) {m_logger->warn(TRTHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlogδ��ʼ��"<<std::endl;}
#define LOGERROR(format,...) if (m_logger) {m_logger->error(TRTHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlogδ��ʼ��"<<std::endl;}
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
	case nvinfer1::DataType::kHALF: return sizeof(float) / 2;		// float��һ��
	case nvinfer1::DataType::kINT8: return sizeof(float) / 4;		// half��һ��
	default: return 4;	// δ֪����ͳһΪ4
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
		std::cout << "yolo����Ϊ��" << std::endl;
		return false;
	}
	if (m_logger == nullptr) {
		std::cout << "spdlog����Ϊ��" << std::endl;
		return false;
	}
	return true;
}

bool ITensorRt::AnalyticalModel(const char* engine_path) {
	//! ��鴫��DML�����ָ���Ƿ���Ч
	if (!CheckPrt()) {
		return false;
	}
	//! ����ģ��
	return InitInterface(engine_path);
}

bool ITensorRt::AnalyticalModel(const std::string engine_path) {
	//! ��鴫��DML�����ָ���Ƿ���Ч
	if (!CheckPrt()) {
		return false;
	}
	//! ����ģ��
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

std::vector<IStates> ITensorRt::getStates() {
	return errors;
}

void ITensorRt::markError(std::string info, bool error = false) {
	MESSAGEBOXA(info.data(), MB_OK);
	errors.push_back(IStates(error, std::string(info)));
}

bool ITensorRt::loadSerializedFile(const char* engine_path) {

	std::ifstream file(UTF8ToAnsi(engine_path).data(), std::ios::binary);
	if (!file.good()) {
		LOGWARN("��ȡengine����,�����ļ���·���Ƿ���ȷ,С����Ϊ�޶�ȡȨ��");
		markError("��ȡengine����,�����ļ���·���Ƿ���ȷ,С����Ϊ�޶�ȡȨ��");
		return false;
	}
	file.seekg(0, file.end);
	char_stream_size = file.tellg();
	if (char_stream_size == 0) {
		LOGWARN("��ȡengine���ݴ���,����engine�ļ���С");
		markError("��ȡengine���ݴ���,����engine�ļ���С");
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
	// ���������豸
	cudaSetDevice(m_equipment);

	// ����engine�ļ�
	if (!loadSerializedFile(engine_path)) {
		return false;
	}

	// ��ʼ���ӿ�
	m_runtime = nvinfer1::createInferRuntime(m_nvlog);
	if (CHECK_TRT(m_runtime)) {
		LOGWARN("���� nvinfer1 Runtimeʧ��");
		markError("���� nvinfer1 Runtimeʧ��");
		return false;
	}

	//! ����engine�������л�
	m_engine = m_runtime->deserializeCudaEngine(char_stream, char_stream_size);
	if (CHECK_TRT(m_engine)) {
		LOGWARN("����Engineʧ��,����������engine�ļ�");
		markError("����Engineʧ��,����������engine�ļ�");
		return false;
	}
	//! ����������
	m_context = m_engine->createExecutionContext();
	if (CHECK_TRT(m_context)) {
		LOGWARN("���� context ʧ��,δ֪ԭ��");
		markError("���� context ʧ��,δ֪ԭ��");
		return false;
	}

	//! �ͷ��ֽ���
	if (char_stream != nullptr) {
		delete[] char_stream;
		char_stream = nullptr;
	}

	//! ����cuda��
	CUDA_CHECK(cudaStreamCreate(&m_stream));
	LOGINFO("cuda�ӿڳ�ʼ�� Done...");

	// ����ģ��
	if (!parseModelInfo()) {
		//! ����ģ�� EX
		//! ��ͨ���ڵ���ע��ʧ��,�����ǲ������Զ����
		//! ����ͨ��ά��ע��
		if (!parseModelInfoEx())  return false;

		LOGINFO("ͨ��parseModelInfoEx ע��ڵ�");
	}
	else {
		LOGINFO("parseModelInfoע��ڵ� Done...");
	}

	// ����host�ڴ�
	input_memory = new float[input_memory_size];
	output_memory = new float[output_memory_size];

	LOGINFO("����host�ڴ� Done...");
	return true;
}

bool ITensorRt::parseModelInfo() {

	// ��ȡָ���Ĳ�����
	input_idx = m_engine->getBindingIndex(*m_yolo->getInputName());
	output_idx = m_engine->getBindingIndex(*m_yolo->getOutputName());

	// ���
	if (input_idx == -1) {
		LOGWARN("�Ҳ�����Ϊ{}������ڵ�", *m_yolo->getInputName());
		markError("�Ҳ�������ڵ�");
		return false;
	}

	if (output_idx == -1) {
		LOGWARN("�Ҳ�����Ϊ{}������ڵ�", *m_yolo->getOutputName());
		markError("�Ҳ�������ڵ�");
		return false;
	}

	gpu_buffers.resize(m_engine->getNbBindings());
	// ע��ڵ�
	for (size_t i = 0; i < m_engine->getNbBindings(); i++) {
		// ��ȡ�ò������shape������
		nvinfer1::Dims dims = m_engine->getBindingDimensions(i);
		nvinfer1::DataType type = m_engine->getBindingDataType(i);
		if (i == input_idx) {
			asserthr(registerInput(dims, type));
		}
		else if (i == output_idx) {
			asserthr(registerOutput(dims, type));
		}
		else {
			// ���ܴ���������ʹ�õĽڵ�,��ҲҪע��,���ڴ�ҲҪ��λ
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
	// ͨ��ά���ҵ�
	if (in && out) {
		return true;
	}

	// δ�ҵ�
	if (!in) {
		LOGWARN("ͨ��ά�Ƚ���ģ��ʧ��,����δ�ҵ�");
	}
	if (!out) {
		LOGWARN("ͨ��ά�Ƚ���ģ��ʧ��,���δ�ҵ�");
	}
	return false;
}

bool ITensorRt::registerInput(nvinfer1::Dims& dims, nvinfer1::DataType& type) {
	// �������Գ�
	if (dims.d[2] != dims.d[3]) {
		LOGWARN("��֧������(image size)���ԳƵ�ģ��");
		markError("��֧������(image size)���ԳƵ�ģ��");
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

	//! ��ӡ
	for (size_t i = 0; i < temp.size(); i++) {
		std::cout << TRTHEAD << "inputά��:" << temp.at(i) << std::endl;
	}

	// ������������
	m_yolo->setInputDims(temp);

	// ��ȡ�����ڴ��С, �ڴ���Դ��λ���
	input_memory_size = volume(dims) * getElementSize(type);

	// ע������ڵ�
	CUDA_CHECK(cudaMalloc(&gpu_buffers[input_idx], input_memory_size));

	// �������־
	LOGINFO("����Shape: {} {} {} {}", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]), tostr(dims.d[3]));
	return true;
}

bool ITensorRt::registerOutput(nvinfer1::Dims& dims, nvinfer1::DataType& type) {
	if (m_yolo->dims_error(dims.d[1], dims.d[2])) {
		LOGWARN("���shape�Ϳ�ܲ�ƥ��.Shape is [ {} {} {} ] ", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]));
		markError("ģ�ͺͿ�ܲ�ƥ��");
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
	//! ��ӡ
	for (size_t i = 0; i < temp.size(); i++) {
		std::cout << TRTHEAD << "outputά��:" << temp.at(i) << std::endl;
	}
	// �����������
	m_yolo->setOutputDims(temp);

	// ��ȡ���ע���ڴ��С
	output_memory_size = volume(dims) * getElementSize(type);

	// ע������ڵ�
	CUDA_CHECK(cudaMalloc(&gpu_buffers[output_idx], output_memory_size));

	// �������־
	LOGINFO("���Shape: {} {} {}", tostr(dims.d[0]), tostr(dims.d[1]), tostr(dims.d[2]));
	return true;
}

void ITensorRt::imageToBlob(cv::Mat& img) {
	// �İ�
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
	// ͬ������
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
	// �첽����
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
	InferExecute();	//! ͬ������
#else
	InferEnqueue();	//! �첽����
#endif
	m_yolo->DecodeOutput(output_memory);
}

void ITensorRt::Release() {
	if (m_stream) cudaStreamDestroy(m_stream);
	for (int i = 0; i < m_engine->getNbBindings(); i++) {   //ѭ���ͷ�
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
	LOGINFO("�ͷ�TensorRt��Դ Done \n\n\n\n");
	delete this;
}

ITensorRt::ITensorRt(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment) :Frame(yolo, logger, equipment) {
	std::cout << "[debug]: TensorRt ����" << std::endl;
}

ITensorRt::~ITensorRt() {
	std::cout << "[debug]: TensorRt ����" << std::endl;
}
