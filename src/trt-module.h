#pragma once
#define CLASS_EXPORT 

#include <cuda_runtime_api.h>
#include <NvInfer.h>
#include "IStates.h"
#include "frame.h"
#include <logging.h>

class ITensorRt final : public Frame {
public:
	//! 解析模型接口 IStates
	bool AnalyticalModel(const char* engine_path) override;

	//! 解析模型接口
	bool AnalyticalModel(const std::string engine_path) override;

	//! 获取错误信息
	IStates getLastErrorInfo() override;

	//! 获取日志对象
	std::vector<IStates> getStates() {
		return errors;
	}

	//! 集预处理,推理,后处理一身的推理接口
	//! 后处理调用yolo配置表的函数
	void Detect(cv::Mat&) override;

	//! 释放
	void Release() override;

private:
	~ITensorRt() {};

	int input_idx = 0;							//! 输入节点索引
	int output_idx = 0;							//! 输入节点索引

	float* input_memory = nullptr;
	float* output_memory = nullptr;

	int64_t input_memory_size = 0;				//! 输入大小(cpu)
	int64_t output_memory_size = 0;				//! 输出大小(cpu)

	size_t char_stream_size = 0;
	char* char_stream = nullptr;							//! engine字节流

	std::vector<void*> gpu_buffers;

	std::vector<IStates> errors;						//! 错误信息对象
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
