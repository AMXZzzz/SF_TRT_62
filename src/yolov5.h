#pragma once
#include "yolo.h"

namespace sf {
	namespace Pattern {
		class YOLOV5 final : public YOLO {		// ��(��һ��)��ģ��
		public:
			// -------------------- API -------------------- //
			//! ��ȡ��ǰ���ñ�����
			sf::Type::YoloType getConfigType() override;

			//! yolov5�ĺ���������yolov7
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

