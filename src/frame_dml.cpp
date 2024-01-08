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
#include <iostream>
#include <vector>
#include "frame_dml.h"

#define MESSAGEBOXA(x,y) MessageBoxA(NULL, x, "DML Error", y)	// 0x00000000L is MB_OK
#define tostr(x) std::to_string(x).c_str()
#define asserthr(x) if(!x) return false;

#define DMLHEAD "[DML]: "
#define LOGINFO(format,...) if (m_logger) {m_logger->info(DMLHEAD ## format, __VA_ARGS__);} else {std::cout<<"spdlogδ��ʼ��"<<std::endl;}
#define LOGWARN(format,...)  if (m_logger) {m_logger->warn(DMLHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlogδ��ʼ��"<<std::endl;}
#define LOGERROR(format,...) if (m_logger) {m_logger->error(DMLHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlogδ��ʼ��"<<std::endl;}

#define CHECKORT(x,y) if (!CheckStatus(x,y)) return false;

static std::wstring String2WString(const std::string& s) {
	std::string strLocale = setlocale(LC_ALL, "");
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;

	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);

	std::wstring wstrResult = wchDest;
	delete[]wchDest;
	setlocale(LC_ALL, strLocale.c_str());

	return wstrResult;
}

static std::string StringToUTF8(const std::string& str) {
	size_t nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	size_t nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;
	return retStr;
}

static std::wstring convertU8andGetWstr(const char* onnx_path) {
	return	String2WString(StringToUTF8(onnx_path));
}

void IDML::markError(char* info, bool sta = false) {
	MESSAGEBOXA(info, MB_OK);
	dmlerrors.push_back(IStates(sta, info));
}

bool IDML::CheckStatus(OrtStatus* status, int line) {
	if (status != NULL) {
		const char* msg = m_ort->GetErrorMessage(status);
		std::cout << msg << " of " << line << std::endl;
		LOGERROR("{} of {}", msg, tostr(line));
		m_ort->ReleaseStatus(status);
		return false;
	}
	return true;
}

bool IDML::CheckPrt() {
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

bool IDML::InitInterface(const wchar_t* onnx_path) {
	CHECKORT(m_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "SuperResolutionA", &m_env), __LINE__);	// �������л���
	CHECKORT(m_ort->CreateSessionOptions(&m_session_options), __LINE__);							// �����Ự�����ļ�
	CHECKORT(m_ort->SetSessionGraphOptimizationLevel(m_session_options, ORT_ENABLE_BASIC), __LINE__);	// �Ż��ȼ�
	CHECKORT(m_ort->DisableMemPattern(m_session_options), __LINE__);			// �ر��ڴ�
	OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_DML(m_session_options, m_equipment);	// �������, sb����	

	CHECKORT(m_ort->CreateSession(m_env, onnx_path, m_session_options, &m_session), __LINE__);	// �����Ự
	LOGINFO("InitInterface Done...");
	// ����ģ��
	asserthr(parseModelInfo());
	return true;
}

bool IDML::parseInput() {

	size_t input_num = 0;
	size_t shape_size = 0;
	char* input_names_temp = nullptr;								
	OrtTypeInfo* input_typeinfo = nullptr;							//����������Ϣ
	const OrtTensorTypeAndShapeInfo* Input_tensor_info = nullptr;   //�����tensor��Ϣ

	// ��ȡ����ڵ�����
	CHECKORT(m_ort->SessionGetInputCount(m_session, &input_num), __LINE__);

	// ��������ڵ�����
	for (size_t i = 0; i < input_num; i++) {
		CHECKORT(m_ort->SessionGetInputName(m_session, i, m_allocator, &input_names_temp), __LINE__);	// ��ȡ�ýڵ�����
		//! Ѱ����ͬ�Ĳ���
		if (strcmp(*(m_yolo->getInputName()), input_names_temp)) {		// strcmp  0:���
			continue;
		}

		// ---------- ��ȡά����Ϣ  ----------
		CHECKORT(m_ort->SessionGetInputTypeInfo(m_session, i, &input_typeinfo), __LINE__);//��ȡ��i��������Ϣ��
		CHECKORT(m_ort->CastTypeInfoToTensorInfo(input_typeinfo, &Input_tensor_info), __LINE__);	//��ȡ����tensor��Ϣ
		CHECKORT(m_ort->GetDimensionsCount(Input_tensor_info, &shape_size), __LINE__);			// ��ȡά�ȴ�С

		std::vector<int64_t> temp;
		temp.resize(shape_size);
		CHECKORT(m_ort->GetDimensions(Input_tensor_info, temp.data(), shape_size), __LINE__); // ��ȡά����Ϣ

		if (temp.empty()) {
			LOGWARN("��ȡ��������ʧ��");
			return false;
		}

		if (temp[2] != temp[3]) {
			LOGWARN("��֧������(image size)���ԳƵ�ģ��");
			markError("��֧������(image size)���ԳƵ�ģ��");
			return false;
		}

		// ����ά��
		for (size_t i = 0; i < shape_size; i++) {
			input_tensor_size = input_tensor_size * temp[i];		//����ά�ȵ��ڴ��С
			std::cout << DMLHEAD << "inputά��:" << temp[i] << std::endl;
		}
		//! ��ά�ȴ��ݵ�yolo����
		m_yolo->setInputDims(temp);

		LOGINFO("��ȡ����Done. Shape is [ {} {} {} {} ] ", tostr(temp[0]), tostr(temp[1]), tostr(temp[2]), tostr(temp[3]));
		return true;
	}

	LOGWARN("��ȡ����ڵ�ʧ�ܣ���ȷ��ģ��������Ϊimage");
	std::cout << "��ȡ����ڵ�ʧ�ܣ���ȷ��ģ��������Ϊimage" << std::endl;
	return false;
}

bool IDML::parseOutput() {

	size_t shape_size = 0;    //ά��
	size_t num_output_nodes = 0;
	char* output_names_temp = nullptr;
	OrtTypeInfo* output_typeinfo = nullptr;     // �����Ϣ
	const OrtTensorTypeAndShapeInfo* output_tensor_info = nullptr;   // tensor��Ϣ
	
	CHECKORT(m_ort->SessionGetOutputCount(m_session, &num_output_nodes), __LINE__);	// ��ȡ���������

	for (size_t i = 0; i < num_output_nodes; i++) {
		// ��ȡ�����
		CHECKORT(m_ort->SessionGetOutputName(m_session, i, m_allocator, &output_names_temp), __LINE__);
		//! �ҵ���ͬ�Ĳ���
		if (strcmp(*(m_yolo->getOutputName()), output_names_temp)) {		// strcmp  0:���
			continue;
		}

		CHECKORT(m_ort->SessionGetOutputTypeInfo(m_session, i, &output_typeinfo), __LINE__);	 // ��ȡ��i���������Ϣ
		CHECKORT(m_ort->CastTypeInfoToTensorInfo(output_typeinfo, &output_tensor_info), __LINE__);				 // ��ȡ���tensor��Ϣ

		CHECKORT(m_ort->GetDimensionsCount(output_tensor_info, &shape_size), __LINE__);

		std::vector<int64_t> temp;
		temp.resize(shape_size);        //������С
		//std::cout << "ά�� shape��" << shape_size << std::endl;
		CHECKORT(m_ort->GetDimensions(output_tensor_info, temp.data(), shape_size), __LINE__);

		if (temp.empty()) {
			std::cout << "[debug]: DML ��ȡ�������ʧ��" << std::endl;
			LOGWARN("��ȡ�������ʧ��");
			return false;
		}
		//! ��ӡ
		for (size_t i = 0; i < shape_size; i++) {
			std::cout<< DMLHEAD << "outputά��:" << temp[i] << std::endl;
		}

		// ���ά��
		if (m_yolo->dims_error(temp[1], temp[2])) {
			LOGWARN("���shape�Ϳ�ܲ�ƥ��. Shape Error: [ {} {} {} ] ", tostr(temp[0]), tostr(temp[1]), tostr(temp[2]));
			markError("ģ�ͺͿ�ܲ�ƥ��");
			std::cout << "[debug]: DML ���shape�Ϳ�ܲ�ƥ��" << std::endl;
			return false;
		}

		// Ϊyolo���󴫵����ά��
		m_yolo->setOutputDims(temp);

		LOGINFO("��ȡ���Done. Shape is [ {} {} {} ] ", tostr(temp[0]), tostr(temp[1]), tostr(temp[2]));
		return true;
	}

	// δ�ҵ�
	std::cout << "�Ҳ�������ڵ�: " << *m_yolo->getOutputName() << std::endl;
	LOGWARN("��ȡ����ڵ�ʧ�ܣ���ȷ��ģ�������Ϊ{}", *m_yolo->getOutputName());

	return false;
}

bool IDML::parseModelInfo() {

	// ��ȡģ����Ϣ
	CHECKORT(m_ort->GetAllocatorWithDefaultOptions(&m_allocator), __LINE__);	// ��ȡ��Ϣ
	//// ��������
	asserthr(parseInput());
	// �������
	asserthr(parseOutput());
	// ����tensor
	CHECKORT(m_ort->CreateTensorAsOrtValue(m_allocator, m_yolo->getInputDims().data(), 4, ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16, &m_input_tensors), __LINE__);
	LOGINFO("��������tensor Done");

	// ����Host�ڴ�
	CHECKORT(m_ort->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &m_memory_info), __LINE__);
	LOGINFO("Create Host memory Done");
	return true;
}

std::vector<IStates> IDML::getStates() {
	return dmlerrors;
}

bool IDML::AnalyticalModel(const char* onnx_path) {
	//! ��鴫��DML�����ָ���Ƿ���Ч
	if (!CheckPrt()) {
		return false;
	}

	//! ����ģ��
	return InitInterface(String2WString(onnx_path).c_str());
}

bool IDML::AnalyticalModel(const std::string onnx_path) {
	//! ���ָ���Ƿ���Ч
	if (!CheckPrt()) {
		return false;
	}
	//! ����ģ��
	return InitInterface(String2WString(onnx_path).c_str());
}

IStates IDML::getLastErrorInfo() {
	if (dmlerrors.empty()) {
		return IStates();
	}
	else {
		int idx = dmlerrors.size() - 1;
		return IStates(dmlerrors[idx].is_success(), dmlerrors[idx].getErrorMessage());
	}
}

void IDML::Detect(cv::Mat& img) {
	cv::Mat blob = cv::dnn::blobFromImage(img, m_yolo->getNormalized(), m_yolo->getImageSize(), NULL, true, false);
	CheckStatus(m_ort->CreateTensorWithDataAsOrtValue(m_memory_info, blob.ptr<float>(), input_tensor_size * sizeof(float), m_yolo->getInputDims().data(), m_yolo->getInputDims().size(), ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, &m_input_tensors), __LINE__);
	CheckStatus(m_ort->Run(m_session, NULL, m_yolo->getInputName(), &m_input_tensors, 1, m_yolo->getOutputName(), 1, &m_output_tensors), __LINE__);
	CheckStatus(m_ort->GetTensorMutableData(m_output_tensors, (void**)&floatarr), __LINE__);
	m_yolo->DecodeOutput(floatarr);
}

void IDML::Release() {
	if (m_env) m_ort->ReleaseEnv(m_env);
	if (m_memory_info) m_ort->ReleaseMemoryInfo(m_memory_info);
	if (m_session) m_ort->ReleaseSession(m_session);
	if (m_session_options) m_ort->ReleaseSessionOptions(m_session_options);
	if (m_input_tensors) m_ort->ReleaseValue(m_input_tensors);
	if (m_output_tensors) m_ort->ReleaseValue(m_output_tensors);
	LOGINFO("�ͷ�DML��� Done...");

	delete this;
}

IDML::IDML(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment) : Frame(yolo, logger, equipment) {
	std::cout << "[debug]: DML ����" << std::endl;
}

IDML::~IDML() {
	std::cout << "[debug]: DML ����" << std::endl;
}
