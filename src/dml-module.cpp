#include <iostream>

#include <vector>
#include "dml-module.h"
#include "sf-trt.h"
#include "utils.h"


#define DMLHEAD "[DML]: "
#define LOGINFO(format,...) if (_logger) {_logger->info(DMLHEAD ## format, __VA_ARGS__);} 
#define LOGWARN(format,...)  if (_logger) {_logger->warn(DMLHEAD ## format, __VA_ARGS__);}
#define LOGERROR(format,...) if (_logger) {_logger->error(DMLHEAD ## format, __VA_ARGS__);}

#define CHECKORT(x,y) if (!CheckStatus(x,y)) return false;

static std::wstring convertU8andGetWstr(const char* onnx_path) {
	return	String2WString(StringToUTF8(onnx_path));
}

void sf::dml::IDML::markError(char* info, State sta) {
	MESSAGEBOXA(info, MB_OK);
	_dmlstates.push_back(IStates(sta, info));
}

bool sf::dml::IDML::CheckStatus(OrtStatus* status, int line) {
	if (status != NULL) {
		const char* msg = _ort->GetErrorMessage(status);
		std::cout << msg << " of " << line << std::endl;
		LOGERROR("{} of {}", msg, tostr(line));
		_ort->ReleaseStatus(status);
		return false;
	}
	return true;
}

bool sf::dml::IDML::InitInterface(const wchar_t* onnx_path) {
	CHECKORT(_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "SuperResolutionA", &_env), __LINE__);	// 创建运行环境
	CHECKORT(_ort->CreateSessionOptions(&_session_options), __LINE__);							// 创建会话配置文件
	CHECKORT(_ort->SetSessionGraphOptimizationLevel(_session_options, ORT_ENABLE_BASIC), __LINE__);	// 优化等级
	CHECKORT(_ort->DisableMemPattern(_session_options), __LINE__);			// 关闭内存
	OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_DML(_session_options, _equipment);	// 算力后端, sb报错	

	CHECKORT(_ort->CreateSession(_env, onnx_path, _session_options, &_session), __LINE__);	// 创建会话
	LOGINFO("InitInterface Done...");
	// 解析模型
	asserthr(parseModelInfo());
	return true;
}

bool sf::dml::IDML::parseInput() {

	size_t input_num = 0;
	size_t shape_size = 0;
	char* input_names_temp = nullptr;
	OrtTypeInfo* input_typeinfo = nullptr;							//输入类型信息
	const OrtTensorTypeAndShapeInfo* Input_tensor_info = nullptr;   //输入的tensor信息


	// 获取输入节点数量
	CHECKORT(_ort->SessionGetInputCount(_session, &input_num), __LINE__);

	// 遍历输入节点索引
	for (size_t i = 0; i < input_num; i++) {
		CHECKORT(_ort->SessionGetInputName(_session, i, _allocator, &input_names_temp), __LINE__);	// 获取该节点名称

		if (strcmp(*(_yolo->getInputName()), input_names_temp)) {		// strcmp  0:相等
			continue;
		}

		// ---------- 获取维度信息  ----------
		CHECKORT(_ort->SessionGetInputTypeInfo(_session, i, &input_typeinfo), __LINE__);//获取第0个输入信息，

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
			markError("不支持输入(image size)不对称的模型", State::DimsNotEqual);
			return false;
		}

		// 处理维度
		for (size_t i = 0; i < shape_size; i++) {
			input_tensor_size = input_tensor_size * temp[i];
			std::cout << "维度:" << temp[i] << std::endl;
		}
		_yolo->setInputDims(temp);

		LOGINFO("获取输入Done. Shape is [ {} {} {} {} ] ", tostr(temp[0]), tostr(temp[1]), tostr(temp[2]), tostr(temp[3]));
		return true;
	}

	LOGWARN("获取输入节点失败，请确保模型输入名为image");
	return false;
}

bool sf::dml::IDML::parseOutput() {

	size_t shape_size = 0;    //维度
	size_t num_output_nodes = 0;
	char* output_names_temp = nullptr;
	OrtTypeInfo* output_typeinfo = nullptr;     // 输出信息
	const OrtTensorTypeAndShapeInfo* output_tensor_info = nullptr;   // tensor信息


	CHECKORT(_ort->SessionGetOutputCount(_session, &num_output_nodes), __LINE__);	// 输出层数量

	for (size_t i = 0; i < num_output_nodes; i++) {
		// 获取输出名
		CHECKORT(_ort->SessionGetOutputName(_session, i, _allocator, &output_names_temp), __LINE__);
		if (strcmp(*(_yolo->getOutputName()), output_names_temp)) {		// strcmp  0:相等
			continue;
		}

		CHECKORT(_ort->SessionGetOutputTypeInfo(_session, i, &output_typeinfo), __LINE__);	 // 获取第i个输出的信息
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
		if (_yolo->dims_error(temp[1], temp[2])) {
			LOGWARN("输出shape和框架不匹配.Shape is [ {} {} {} ] ", tostr(temp[0]), tostr(temp[1]), tostr(temp[2]));
			markError("模型和框架不匹配", State::FrameNotMatch);
			return false;
		}

		// 为配置表初始化输出维度
		_yolo->setOutputDims(temp);



		LOGINFO("获取输出Done. Shape is [ {} {} {} ] ", tostr(temp[0]), tostr(temp[1]), tostr(temp[2]));
		return true;
	}
	std::cout << "找不到节点: " << *_yolo->getOutputName() << std::endl;
	LOGWARN("获取输出节点失败，请确保模型输出名为output");
	// 未找到
	return false;
}

bool sf::dml::IDML::parseModelInfo() {

	// 获取模型信息
	CHECKORT(_ort->GetAllocatorWithDefaultOptions(&_allocator), __LINE__);	// 获取信息
	//// 解析输入
	asserthr(parseInput());
	// 解析输出
	asserthr(parseOutput());

	// 创建tensor
	CHECKORT(_ort->CreateTensorAsOrtValue(_allocator, _yolo->getInputDims().data(), 4, ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16, &_input_tensors), __LINE__);
	LOGINFO("创建输入tensor Done");

	// 创建Host内存
	CHECKORT(_ort->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &_memory_info), __LINE__);
	LOGINFO("Create cpu memory Done");
	return true;
}

bool sf::dml::IDML::AnalyticalModel(const wchar_t* onnx_path) {
	return InitInterface(onnx_path);
}

bool sf::dml::IDML::AnalyticalModel(const char* onnx_path) {
	return InitInterface(String2WString(onnx_path).c_str());
}

bool sf::dml::IDML::AnalyticalModel(const std::string onnx_path) {
	return InitInterface(String2WString(onnx_path).c_str());
}

IStates sf::dml::IDML::getLastErrorInfo() {
	if (_dmlstates.empty()) {
		return IStates(State::Success, "没有错误");
	} else {
		int idx = _dmlstates.size() - 1;
		return IStates(_dmlstates[idx].err_code(), _dmlstates[idx].msg());
	}	
}

void sf::dml::IDML::Detect(cv::Mat& img) {

	cv::Mat blob = cv::dnn::blobFromImage(img, _yolo->getNormalized(), _yolo->getImageSize(), NULL, true, false);

	// 存在参数为获取 如Process
	CheckStatus(_ort->CreateTensorWithDataAsOrtValue(_memory_info, blob.ptr<float>(), input_tensor_size * sizeof(float), _yolo->getInputDims().data(), _yolo->getInputDims().size(), ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, &_input_tensors), __LINE__);
	CheckStatus(_ort->Run(_session, NULL, _yolo->getInputName(), &_input_tensors, 1, _yolo->getOutputName(), 1, &_output_tensors), __LINE__);
	CheckStatus(_ort->GetTensorMutableData(_output_tensors, (void**)&floatarr), __LINE__);
	_yolo->DecodeOutput(floatarr, img);
}

void sf::dml::IDML::Release() {
	{
		if (_env) _ort->ReleaseEnv(_env);
		if (_memory_info) _ort->ReleaseMemoryInfo(_memory_info);
		if (_session) _ort->ReleaseSession(_session);
		if (_session_options) _ort->ReleaseSessionOptions(_session_options);
		if (_input_tensors) _ort->ReleaseValue(_input_tensors);
		if (_output_tensors) _ort->ReleaseValue(_output_tensors);
		LOGINFO("释放DML框架 Done");
		delete this;
	}
}

