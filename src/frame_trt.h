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
* [开源地址]:https://github.com/AMXZzzz/SF_TRT_62.git
* [日期]: 2023/10/26
*/
#pragma once
#include <cuda_runtime_api.h>
#include <NvInfer.h>
#include <logging.h>

#include "frame_base.h"

class ITensorRt :public Frame {
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
	bool CheckPrt();

public:
	ITensorRt(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment) :Frame(yolo, logger, equipment) {}

};