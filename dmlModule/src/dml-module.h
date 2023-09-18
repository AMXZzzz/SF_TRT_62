#pragma once
#include <windows.h>
#include <dml/dml_provider_factory.h>
#include <opencv2/opencv.hpp>
#include "frame.h"

class SF_CLASS IDML final : public Frame{
public:
	//  --------------------------- ����API --------------------------- // IStates

	//! ����ģ��
	bool AnalyticalModel(const char* onnx_path) override;

	//! ����ģ��
	bool AnalyticalModel(const std::string onnx_path) override;

	//! ��ȡ��������־
	IStates getLastErrorInfo() override;
			
	//! ��Ԥ����,����,����һ�������ӿ�
	void Detect(cv::Mat& img) override;

	//! �ͷ�����
	void Release() override;

	//! ��ȡDML��־����
	std::vector<IStates> getStates();

private:		
	std::vector<IStates> dmlerrors;
	size_t input_tensor_size = 1;						//! �������ݴ�С

	float* floatarr = nullptr;
	OrtEnv* m_env = nullptr;								
	OrtSessionOptions* m_session_options = nullptr;
	OrtSession* m_session = nullptr;
	OrtMemoryInfo* m_memory_info = nullptr;
	OrtAllocator* m_allocator = nullptr;
	OrtValue* m_input_tensors = nullptr;		// ����tensor
	OrtValue* m_output_tensors = nullptr;		// ���tensor
	const OrtApi* _ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);	// api

	
	// Function
	bool CheckStatus(OrtStatus* status, int line);
	bool parseInput();
	bool parseOutput();
	bool parseModelInfo();
	bool InitInterface(const wchar_t* onn_path);
	void markError(char* info, bool );

public:
	// ������!�б��ʼ������!
	IDML(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment) :Frame(yolo, logger, equipment) {}
	~IDML() {};
};
