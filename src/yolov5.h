#pragma once
#include "yolo.h"

namespace sf {
	namespace Pattern {
		class YOLOV5 final : public YOLO {		// 底(第一声)层模块
		public:
			// -------------------- API -------------------- //
			//! 获取当前配置表属性
			sf::Type::YoloType getConfigType() override;

			//! yolov5的后处理，适用于yolov7
			void DecodeOutput(float* output, cv::Mat& img) override;

			void Release() {
				delete this;
			}
		public:
			YOLOV5(float* conf, float* iou, Process* process) : YOLO(conf, iou, process) {};
			~YOLOV5() {};
		};
	}
}

