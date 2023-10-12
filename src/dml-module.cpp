#define CLASS_EXPORT 
#include <iostream>
#include <vector>
#include "dml-module.h"

#define MESSAGEBOXA(x,y) MessageBoxA(NULL, x, "DML Error", y)	// 0x00000000L is MB_OK
#define tostr(x) std::to_string(x).c_str()
#define asserthr(x) if(!x) return false;

#define DMLHEAD "[DML]: "
#define LOGINFO(format,...) if (m_logger) {m_logger->info(DMLHEAD ## format, __VA_ARGS__);} 
#define LOGWARN(format,...)  if (m_logger) {m_logger->warn(DMLHEAD ## format, __VA_ARGS__);}
#define LOGERROR(format,...) if (m_logger) {m_logger->error(DMLHEAD ## format, __VA_ARGS__);}

#define CHECKORT(x,y) if (!CheckStatus(x,y)) return false;

std::wstring String2WString(const std::string& s) {
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

std::string StringToUTF8(const std::string& str) {
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
		const char* msg = _ort->GetErrorMessage(status);
		std::cout << msg << " of " << line << std::endl;
		LOGERROR("{} of {}", msg, tostr(line));
		_ort->ReleaseStatus(status);
		return false;
	}
	return true;
}

bool IDML::InitInterface(const wchar_t* onnx_path) {
	CHECKORT(_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "SuperResolutionA", &m_env), __LINE__);	// 创建运行环境
	CHECKORT(_ort->CreateSessionOptions(&m_session_options), __LINE__);							// 创建会话配置文件
	CHECKORT(_ort->SetSessionGraphOptimizationLevel(m_session_options, ORT_ENABLE_BASIC), __LINE__);	// 优化等级
	CHECKORT(_ort->DisableMemPattern(m_session_options), __LINE__);			// 关闭内存
	OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_DML(m_session_options, m_equipment);	// 算力后端, sb报错	

	CHECKORT(_ort->CreateSession(m_env, onnx_path, m_session_options, &m_session), __LINE__);	// 创建会话
	LOGINFO("InitInterface Done...");
	// 解析模型
	asserthr(parseModelInfo());
	return true;
}

bool IDML::parseInput() {

	size_t input_num = 0;
	size_t shape_size = 0;
	char* input_names_temp = nullptr;
	OrtTypeInfo* input_typeinfo = nullptr;							//输入类型信息
	const OrtTensorTypeAndShapeInfo* Input_tensor_info = nullptr;   //输入的tensor信息


	// 获取输入节点数量
	CHECKORT(_ort->SessionGetInputCount(m_session, &input_num), __LINE__);

	// 遍历输入节点索引
	for (size_t i = 0; i < input_num; i++) {
		CHECKORT(_ort->SessionGetInputName(m_session, i, m_allocator, &input_names_temp), __LINE__);	// 获取该节点名称

		if (strcmp(*(m_yolo->getInputName()), input_names_temp)) {		// strcmp  0:相等
			continue;
		}

		// ---------- 获取维度信息  ----------
		CHECKORT(_ort->SessionGetInputTypeInfo(m_session, i, &input_typeinfo), __LINE__);//获取第0个输入信息，

		CHECKORT(_ort->CastTypeInfoToTensorInfo(input_typeinfo, &Input_tensor_info), __LINE__);	//获取输入tensor信息
		CHECKORT(_ort->GetDimensionsCount(Input_tensor_info, &shape_size), __LINE__);			// 获取维度大小

		std::vector<int64_t> temp;
		temp.resize(shape_size);
		CHECKORT(_ort->GetDimensions(Input_tensor_info, temp.data(), shape_size), __LINE__); // 获取个维度信息

		if (temp.empty()) {
			LOGWARN("获取输入数据失败");
			return false;
		}

		if (temp[2] != temp[3]) {
			LOGWARN("不支持输入(image size)不对称的模型");
			markError("不支持输入(image size)不对称的模型");
			return false;
		}

		// 处理维度
		for (size_t i = 0; i < shape_size; i++) {
			input_tensor_size = input_tensor_size * temp[i];
			std::cout << "维度:" << temp[i] << std::endl;
		}
		m_yolo->setInputDims(temp);

		LOGINFO("获取输入Done. Shape is [ {} {} {} {} ] ", tostr(temp[0]), tostr(temp[1]), tostr(temp[2]), tostr(temp[3]));
		return true;
	}

	LOGWARN("获取输入节点失败，请确保模型输入名为image");
	return false;
}

bool IDML::parseOutput() {

	size_t shape_size = 0;    //维度
	size_t num_output_nodes = 0;
	char* output_names_temp = nullptr;
	OrtTypeInfo* output_typeinfo = nullptr;     // 输出信息
	const OrtTensorTypeAndShapeInfo* output_tensor_info = nullptr;   // tensor信息


	CHECKORT(_ort->SessionGetOutputCount(m_session, &num_output_nodes), __LINE__);	// 输出层数量

	for (size_t i = 0; i < num_output_nodes; i++) {
		// 获取输出名
		CHECKORT(_ort->SessionGetOutputName(m_session, i, m_allocator, &output_names_temp), __LINE__);
		if (strcmp(*(m_yolo->getOutputName()), output_names_temp)) {		// strcmp  0:相等
			continue;
		}

		CHECKORT(_ort->SessionGetOutputTypeInfo(m_session, i, &output_typeinfo), __LINE__);	 // 获取第i个输出的信息
		CHECKORT(_ort->CastTypeInfoToTensorInfo(output_typeinfo, &output_tensor_info), __LINE__);				 // 获取输出tensor信息

		CHECKORT(_ort->GetDimensionsCount(output_tensor_info, &shape_size), __LINE__);

		std::vector<int64_t> temp;
		temp.resize(shape_size);        //容器大小
		std::cout << "维度shape：" << shape_size << std::endl;
		CHECKORT(_ort->GetDimensions(output_tensor_info, temp.data(), shape_size), __LINE__);

		if (temp.empty()) {
			LOGWARN("获取输出数据失败");
			return false;
		}

		for (size_t i = 0; i < shape_size; i++) {
			std::cout << "维度:" << temp[i] << std::endl;
		}

		// 检查
		if (m_yolo->dims_error(temp[1], temp[2])) {
			LOGWARN("输出shape和框架不匹配.Shape is [ {} {} {} ] ", tostr(temp[0]), tostr(temp[1]), tostr(temp[2]));
			markError("模型和框架不匹配");
			return false;
		}

		// 为yolo配置表初始化输出维度
		m_yolo->setOutputDims(temp);

		LOGINFO("获取输出Done. Shape is [ {} {} {} ] ", tostr(temp[0]), tostr(temp[1]), tostr(temp[2]));
		return true;
	}
	std::cout << "找不到节点: " << *m_yolo->getOutputName() << std::endl;
	LOGWARN("获取输出节点失败，请确保模型输出名为output");
	// 未找到
	return false;
}

bool IDML::parseModelInfo() {

	// 获取模型信息
	CHECKORT(_ort->GetAllocatorWithDefaultOptions(&m_allocator), __LINE__);	// 获取信息
	//// 解析输入
	asserthr(parseInput());
	// 解析输出
	asserthr(parseOutput());

	// 创建tensor
	CHECKORT(_ort->CreateTensorAsOrtValue(m_allocator, m_yolo->getInputDims().data(), 4, ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16, &m_input_tensors), __LINE__);
	LOGINFO("创建输入tensor Done");

	// 创建Host内存
	CHECKORT(_ort->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &m_memory_info), __LINE__);
	LOGINFO("Create cpu memory Done");
	return true;
}

bool IDML::AnalyticalModel(const char* onnx_path) {
	return InitInterface(String2WString(onnx_path).c_str());
}

bool IDML::AnalyticalModel(const std::string onnx_path) {
	return InitInterface(String2WString(onnx_path).c_str());
}

std::vector<IStates> IDML::getStates() {
	return dmlerrors;
}

IStates IDML::getLastErrorInfo() {
	if (dmlerrors.empty()) {
		return IStates();
	} else {
		int idx = dmlerrors.size() - 1;
		return IStates(dmlerrors[idx].is_success(), dmlerrors[idx].getErrorInfo());
	}
}

void IDML::Detect(cv::Mat& img) {
	cv::Mat blob = cv::dnn::blobFromImage(img, m_yolo->getNormalized(), m_yolo->getImageSize(), NULL, true, false);
	CheckStatus(_ort->CreateTensorWithDataAsOrtValue(m_memory_info, blob.ptr<float>(), input_tensor_size * sizeof(float), m_yolo->getInputDims().data(), m_yolo->getInputDims().size(), ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, &m_input_tensors), __LINE__);
	CheckStatus(_ort->Run(m_session, NULL, m_yolo->getInputName(), &m_input_tensors, 1, m_yolo->getOutputName(), 1, &m_output_tensors), __LINE__);
	CheckStatus(_ort->GetTensorMutableData(m_output_tensors, (void**)&floatarr), __LINE__);
	m_yolo->DecodeOutput(floatarr);
}

void IDML::Release() {
	if (m_env) _ort->ReleaseEnv(m_env);
	if (m_memory_info) _ort->ReleaseMemoryInfo(m_memory_info);
	if (m_session) _ort->ReleaseSession(m_session);
	if (m_session_options) _ort->ReleaseSessionOptions(m_session_options);
	if (m_input_tensors) _ort->ReleaseValue(m_input_tensors);
	if (m_output_tensors) _ort->ReleaseValue(m_output_tensors);
	LOGINFO("释放DML框架 Done");
	delete this;
}
