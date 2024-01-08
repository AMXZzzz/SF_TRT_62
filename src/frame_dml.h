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
* [地址]:https://github.com/AMXZzzz/SF_TRT_62.git
* [日期]: 2023/10/26
*/
#pragma once
#include <dml_provider_factory.h>
#include "frame_base.h"

class IDML final : public Frame {
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

	//! 获取DML所有日志消息
	std::vector<IStates> getStates();

	//! 释放
	void Release() override;

private:
	IDML();
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