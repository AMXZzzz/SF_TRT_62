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
#pragma once
#include <dml_provider_factory.h>
#include "frame_base.h"

class IDML final : public Frame {
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

	//! ��ȡDML������־��Ϣ
	std::vector<IStates> getStates();

	//! �ͷ�
	void Release() override;

private:
	IDML();
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
	const OrtApi* m_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);	// api

	// Function
	bool CheckStatus(OrtStatus* status, int line);
	bool CheckPrt();
	bool parseInput();
	bool parseOutput();
	bool parseModelInfo();
	bool InitInterface(const wchar_t* onn_path);
	void markError(char* info, bool);

public:
	IDML(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment);
	~IDML();
};