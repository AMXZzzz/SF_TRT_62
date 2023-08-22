#pragma once
#include "yolo.h"

namespace sf {
	namespace Pattern {

		class YOLOX final :public YOLO {	// 底(第一声)层模块
		public:
			YOLOX(float* conf, float* iou, bool* showWindow, Process* process) : YOLO(conf, iou, showWindow, process) {};
			~YOLOX() {};
			
			//! 获取归一化的值
			const float getNormalized() override;

			sf::Type::YoloType getConfigType() override;

			// 解码
			void DecodeOutput(float* output, cv::Mat& img) override;

			void generate_grids_and_stride();

			void Release() {
				delete this;
			}
		private:
			struct GridAndStride {				// 网格维度
				int gh;
				int gw;
				int stride;
			};
			std::vector<GridAndStride> grid_strides;	// 网格
			float Normalized = 1.f;

		};

	}
}