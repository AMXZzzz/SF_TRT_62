#pragma once
#define CLASS_EXPORT 

#include <cuda_runtime_api.h>
#include <NvInfer.h>
#include "IStates.h"
#include "frame.h"
#include <logging.h>

class ITensorRt final : public Frame {
public:
	//! ����ģ�ͽӿ� IStates
	bool AnalyticalModel(const char* engine_path) override;

	//! ����ģ�ͽӿ�
	bool AnalyticalModel(const std::string engine_path) override;

	//! ��ȡ������Ϣ
	IStates getLastErrorInfo() override;

	//! ��ȡ��־����
	std::vector<IStates> getStates() {
		return errors;
	}

	//! ��Ԥ����,����,����һ�������ӿ�
	//! �������yolo���ñ�ĺ���
	void Detect(cv::Mat&) override;

	//! �ͷ�
	void Release() override;

private:
	~ITensorRt() {};

	int input_idx = 0;							//! ����ڵ�����
	int output_idx = 0;							//! ����ڵ�����

	float* input_memory = nullptr;
	float* output_memory = nullptr;

	int64_t input_memory_size = 0;				//! �����С(cpu)
	int64_t output_memory_size = 0;				//! �����С(cpu)

	size_t char_stream_size = 0;
	char* char_stream = nullptr;							//! engine�ֽ���

	std::vector<void*> gpu_buffers;

	std::vector<IStates> errors;						//! ������Ϣ����
	nvLogger m_nvlog;
	cudaStream_t m_stream = nullptr;
	nvinfer1::IRuntime* m_runtime = nullptr;
	nvinfer1::ICudaEngine* m_engine = nullptr;
	nvinfer1::IExecutionContext* m_context = nullptr;

	bool InitInterface(const char* engine_path);
	bool loadSerializedFile(const char* engine_path);
	bool parseModelInfo();
	bool parseModelInfoEx();
	bool registerInput(nvinfer1::Dims& dims, nvinfer1::DataType& type);
	bool registerOutput(nvinfer1::Dims& dims, nvinfer1::DataType& type);
	void imageToBlob(cv::Mat& img);
	void InferExecute();
	void InferEnqueue();
	void markError(std::string, bool);


public:
	ITensorRt(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment) :Frame(yolo, logger, equipment) {}

};
