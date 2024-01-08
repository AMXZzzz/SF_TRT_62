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
* [��Դ��ַ]:https://github.com/AMXZzzz/SF_TRT_62.git
* [����]: 2023/10/26
*/
#pragma once
#include <cuda_runtime_api.h>
#include <NvInfer.h>
#include <logging.h>

#include "frame_base.h"

class ITensorRt :public Frame {
public:

	//! ����ģ�ͽӿ� IStates
	bool AnalyticalModel(const char* engine_path) override;

	//! ����ģ�ͽӿ�
	bool AnalyticalModel(const std::string engine_path) override;

	//! ��ȡ������Ϣ
	IStates getLastErrorInfo() override;

	//! ��ȡ��־����
	std::vector<IStates> getStates();

	//! ��Ԥ����,����,����һ�������ӿ�
	void Detect(cv::Mat&) override;

	//! �ͷ�
	void Release() override;

private:


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
	bool CheckPrt();

public:
	ITensorRt(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment);
	~ITensorRt();
};