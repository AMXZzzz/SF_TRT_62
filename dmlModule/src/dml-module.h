#pragma once
#include <windows.h>
#include <dml/dml_provider_factory.h>
#include <opencv2/opencv.hpp>
#include "frame.h"

class SF_CLASS IDML final : public Frame{
public:
	//  --------------------------- 对外API --------------------------- // IStates

	//! 解析模型
	bool AnalyticalModel(const char* onnx_path) override;

	//! 解析模型
	bool AnalyticalModel(const std::string onnx_path) override;

	//! 获取最后错误日志
	IStates getLastErrorInfo() override;
			
	//! 集预处理,推理,后处理一身的推理接口
	void Detect(cv::Mat& img) override;

	//! 释放自身
	void Release() override;

	//! 获取DML日志对象
	std::vector<IStates> getStates();

private:		
	std::vector<IStates> dmlerrors;
	size_t input_tensor_size = 1;						//! 输入数据大小

	float* floatarr = nullptr;
	OrtEnv* m_env = nullptr;								
	OrtSessionOptions* m_session_options = nullptr;
	OrtSession* m_session = nullptr;
	OrtMemoryInfo* m_memory_info = nullptr;
	OrtAllocator* m_allocator = nullptr;
	OrtValue* m_input_tensors = nullptr;		// 输入tensor
	OrtValue* m_output_tensors = nullptr;		// 输出tensor
	const OrtApi* _ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);	// api

	
	// Function
	bool CheckStatus(OrtStatus* status, int line);
	bool parseInput();
	bool parseOutput();
	bool parseModelInfo();
	bool InitInterface(const wchar_t* onn_path);
	void markError(char* info, bool );

public:
	// 派生类!列表初始化基类!
	IDML(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment) :Frame(yolo, logger, equipment) {}
	~IDML() {};
};
