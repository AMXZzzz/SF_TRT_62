#pragma once
#include <windows.h>
#include <dml_provider_factory.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <opencv2/opencv.hpp>
#include "yolo.h"
#include "frame-base.h"

namespace sf {
	namespace dml {

		class IDML final :  public Frame{	// ��(��һ��)��ģ��
		public:
			//  --------------------------- ����API --------------------------- //
			//! ����ģ��
			//! wchar���ͣ����������·��
			bool AnalyticalModel(const wchar_t* onnx_path);

			//! ����ģ��
			//! char����,���������·��
			bool AnalyticalModel(const char* onnx_path) override;

			//! ����ģ��
			//! char����,���������·��
			bool AnalyticalModel(const std::string onnx_path) override;

			//! ��ȡ��־����
			std::vector<IStates> getStates() {
				return _dmlstates;
			}

			//! ��ȡ��������־
			IStates getLastErrorInfo() override;

			//! ��Ԥ����,����,����һ�������ӿ�
			//! �������yolo���ñ�ĺ���
			void Detect(cv::Mat& img) override;

			//! �ͷ�����
			void Release() override;

		private:		
			std::vector<IStates> _dmlstates;
			size_t input_tensor_size = 1;						//! �������ݴ�С

			float* floatarr = nullptr;
			OrtEnv* _env = nullptr;								
			OrtSessionOptions* _session_options = nullptr;
			OrtSession* _session = nullptr;
			OrtMemoryInfo* _memory_info = nullptr;
			OrtAllocator* _allocator = nullptr;
			OrtValue* _input_tensors = nullptr;		// ����tensor
			OrtValue* _output_tensors = nullptr;		// ���tensor
			const OrtApi* _ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);	// api

	
			// Function
			bool CheckStatus(OrtStatus* status, int line);
			bool parseInput();
			bool parseOutput();
			bool parseModelInfo();
			bool InitInterface(const wchar_t* onn_path);
			void markError(char* info, State sta);


		public:
			// ������!�б��ʼ������!
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
				if (!_yolo) 	return IStates(State::UNKONEERR ,"yolo���ñ����");
				if (!_logger) 	return IStates(State::UNKONEERR ,"��־����");
				return IStates();
			};
			DMLFactory() {};
		};
	}
}