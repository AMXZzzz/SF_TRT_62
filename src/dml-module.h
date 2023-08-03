#pragma once
#include <windows.h>
#include <dml_provider_factory.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <opencv2/opencv.hpp>
#include "yolo.h"
#include "frame-base.h"

namespace sf {
	namespace dml {

		class IDML final :  public Frame{	// 底(第一声)层模块
		public:
			//  --------------------------- 对外API --------------------------- //
			//! 解析模型
			//! wchar类型，可以是相对路径
			bool AnalyticalModel(const wchar_t* onnx_path);

			//! 解析模型
			//! char类型,可以是相对路径
			bool AnalyticalModel(const char* onnx_path) override;

			//! 解析模型
			//! char类型,可以是相对路径
			bool AnalyticalModel(const std::string onnx_path) override;

			//! 获取日志对象
			std::vector<IStates> getStates() {
				return _dmlstates;
			}

			//! 获取最后错误日志
			IStates getLastErrorInfo() override;

			//! 集预处理,推理,后处理一身的推理接口
			//! 后处理调用yolo配置表的函数
			void Detect(cv::Mat& img) override;

			//! 释放自身
			void Release() override;

		private:		
			std::vector<IStates> _dmlstates;
			size_t input_tensor_size = 1;						//! 输入数据大小

			float* floatarr = nullptr;
			OrtEnv* _env = nullptr;								
			OrtSessionOptions* _session_options = nullptr;
			OrtSession* _session = nullptr;
			OrtMemoryInfo* _memory_info = nullptr;
			OrtAllocator* _allocator = nullptr;
			OrtValue* _input_tensors = nullptr;		// 输入tensor
			OrtValue* _output_tensors = nullptr;		// 输出tensor
			const OrtApi* _ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);	// api

	
			// Function
			bool CheckStatus(OrtStatus* status, int line);
			bool parseInput();
			bool parseOutput();
			bool parseModelInfo();
			bool InitInterface(const wchar_t* onn_path);
			void markError(char* info, State sta);


		public:
			// 派生类!列表初始化基类!
			IDML(YOLO* yolo):Frame(yolo)  {} 
			IDML(YOLO* yolo, std::shared_ptr<spdlog::logger> logger) :Frame(yolo, logger) {}
			IDML(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment) :Frame(yolo, logger, equipment) {}
			~IDML() {};
		};


		class DMLFactory final : public FrameFactory {
		public:
			static DMLFactory* createDMLFactory() {
				return new DMLFactory();
			}

			IStates QueryInterface(void** dml) override {
				IStates hr = checkFrameMemberPtr();
				if (hr.is_error()) {
					return hr;
				}
				*dml = new sf::dml::IDML(_yolo, _logger, _equipment);
				return IStates();
			}

			void Release() override {
				delete this;
			}
		private:
			IStates checkFrameMemberPtr() {
				if (!_yolo) 	return IStates(State::UNKONEERR ,"yolo配置表错误");
				if (!_logger) 	return IStates(State::UNKONEERR ,"日志错误");
				return IStates();
			};
			DMLFactory() {};
		};
	}
}