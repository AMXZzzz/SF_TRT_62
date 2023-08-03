#pragma once
#include <cuda_runtime_api.h>
#include <NvInfer.h>
#include "utils.h"
#include "logging.h"
#include "frame-base.h"

namespace sf {
	namespace trt {

		class ITensorRt final : public Frame {
		public:
			//! ����ģ�ͽӿ�
			bool AnalyticalModel(const char* engine_path) override;

			//! ����ģ�ͽӿ�
			bool AnalyticalModel(const std::string engine_path) override;

			//! ��ȡ������Ϣ
			IStates getLastErrorInfo() override;

			//! ��ȡ��־����
			std::vector<IStates> getStates() {
				return _trtstates;
			}

			//! ��Ԥ����,����,����һ�������ӿ�
			//! �������yolo���ñ�ĺ���
			void Detect(cv::Mat&) override;

			//! �ͷ�
			void Release() override ;

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
			//void* gpu_buffers[5] = { NULL, NULL ,NULL ,NULL,NULL };

			std::vector<IStates> _trtstates;						//! ������Ϣ����
			nvLogger _nvlog;
			cudaStream_t _stream = nullptr;
			nvinfer1::IRuntime* _runtime = nullptr;
			nvinfer1::ICudaEngine* _engine = nullptr;
			nvinfer1::IExecutionContext* _context = nullptr;

			bool InitInterface(const char* engine_path);
			bool loadSerializedFile(const char* engine_path);
			bool parseModelInfo();
			bool parseModelInfoEx();
			bool registerInput(nvinfer1::Dims& dims, nvinfer1::DataType& type);
			bool registerOutput(nvinfer1::Dims& dims, nvinfer1::DataType& type);
			void imageToBlob(cv::Mat& img);
			void InferExecute();
			void InferEnqueue();
			void markError(std::string info, State sta);
		

		public:
			ITensorRt(YOLO* yolo) :Frame(yolo) {}
			ITensorRt(YOLO* yolo, std::shared_ptr<spdlog::logger> logger) :Frame(yolo, logger) {}
			ITensorRt(YOLO* yolo, std::shared_ptr<spdlog::logger> logger, int equipment) :Frame(yolo, logger, equipment) {}
			~ITensorRt() {};
		};

		class TRTFactory final : public FrameFactory {
		public:
			static TRTFactory* createTRTFactory() {
				return new TRTFactory();
			}
			IStates QueryInterface(void** trt) override;

			void Release() override {
				delete this;
			}
		private:
			IStates checkFrameMemberPtr();
			TRTFactory() {};		// ���ع���
		};
	}
}
