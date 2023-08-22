#pragma once
#include "yolo.h"

namespace sf {
	namespace Pattern {

		class YOLOV8 final : public YOLO {	// ��(��һ��)��ģ��
		public:
			YOLOV8(float* conf, float* iou, bool* showWindow, Process* process) : YOLO(conf, iou, showWindow, process) {};
			~YOLOV8() {};

			//! �ͷ�
			void Release() {
				delete this;
			}

			//! ��ȡ����ڵ������
			//! ����ָ��
			const char** getOutputName() override;

			//! ��ȡ���ñ������
			sf::Type::YoloType getConfigType() override;


			//! ��֤ά���Ƿ���ȷ
			bool dims_error(int64_t dim1, int64_t dim2) override;

			//! ����
			void DecodeOutput(float* output, cv::Mat& img) override;
		private:
			// YOLOV8�Լ��Ĳ���
			const char* output_name[1] = { "output0" };
		};
	}
}