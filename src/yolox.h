#pragma once
#include "yolo.h"

namespace sf {
	namespace Pattern {

		class YOLOX final :public YOLO {	// ��(��һ��)��ģ��
		public:
			YOLOX(float* conf, float* iou, bool* showWindow, Process* process) : YOLO(conf, iou, showWindow, process) {};
			~YOLOX() {};
			
			//! ��ȡ��һ����ֵ
			const float getNormalized() override;

			sf::Type::YoloType getConfigType() override;

			// ����
			void DecodeOutput(float* output, cv::Mat& img) override;

			void generate_grids_and_stride();

			void Release() {
				delete this;
			}
		private:
			struct GridAndStride {				// ����ά��
				int gh;
				int gw;
				int stride;
			};
			std::vector<GridAndStride> grid_strides;	// ����
			float Normalized = 1.f;

		};

	}
}