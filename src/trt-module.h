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
			//! 解析模型接口
			bool AnalyticalModel(const char* engine_path) override;

			//! 解析模型接口
			bool AnalyticalModel(const std::string engine_path) override;

			//! 获取错误信息
			IStates getLastErrorInfo() override;

			//! 获取日志对象
			std::vector<IStates> getStates() {
				return _trtstates;
			}

			//! 集预处理,推理,后处理一身的推理接口
			//! 后处理调用yolo配置表的函数
			void Detect(cv::Mat&) override;

			//! 释放
			void Release() override ;

		private:
			int input_idx = 0;							//! 输入节点索引
			int output_idx = 0;							//! 输入节点索引

			float* input_memory = nullptr;
			float* output_memory = nullptr;

			int64_t input_memory_size = 0;				//! 输入大小(cpu)
			int64_t output_memory_size = 0;				//! 输出大小(cpu)

			size_t char_stream_size = 0;
			char* char_stream = nullptr;							//! engine字节流

			std::vector<void*> gpu_buffers;
			//void* gpu_buffers[5] = { NULL, NULL ,NULL ,NULL,NULL };

			std::vector<IStates> _trtstates;						//! 错误信息对象
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
			TRTFactory() {};		// 隐藏构造
		};
	}
}
