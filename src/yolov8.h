#pragma once
#include "yolo.h"

namespace sf {
	namespace Pattern {

		class YOLOV8 final : public YOLO {	// 底(第一声)层模块
		public:
			YOLOV8(float* conf, float* iou, Process* process) :YOLO(conf, iou, process) {};
			~YOLOV8() {};

			//! 释放
			void Release() {
				delete this;
			}

			//! 获取输出节点的名称
			//! 二级指针
			const char** getOutputName() override;

			//! 获取配置表的类型
			sf::Type::YoloType getConfigType() override;


			//! 验证维度是否正确
			bool dims_error(int64_t dim1, int64_t dim2) override;

			//! 解码
			void DecodeOutput(float* output, cv::Mat& img) override;
		private:
			// YOLOV8自己的层名
			const char* output_name[1] = { "output0" };
		};
	}
}