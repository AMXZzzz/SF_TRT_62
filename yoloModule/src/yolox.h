#pragma once
#define API_EXPORT
#include "yolo.h"

class YOLOX final :public YOLO {	
public:
	//! 传入置信度指针,iou指针,处理结构体
	YOLOX(float* conf, float* iou, Process* process) : YOLO(conf, iou, process) {};

	//! 获取归一化的值
	const float getNormalized() override;

	//! 获取配置表类型
	sf::Type::YoloType getYoloType() override;

	//! 解码重写
	void DecodeOutput(float* output) override;

	//! 主动释放
	void Release();

private:
	//! yolox的落点函数
	void generate_grids_and_stride();

	~YOLOX() {};

	struct GridAndStride {				// 网格维度
		int gh;
		int gw;
		int stride;
	};

	std::vector<GridAndStride> grid_strides;	// 网格
	float Normalized = 1.f;

};

