#pragma once
#define API_EXPORT
#include "yolo.h"


class  YOLOV5 final : public YOLO {
public:
	//! 有参构造
	YOLOV5(float* conf, float* iou, Process* process) : YOLO(conf, iou, process) {};

	//! 获取当前配置表属性
	sf::Type::YoloType getYoloType() override;

	//! yolov5的后处理，适用于yolov7
	void DecodeOutput(float* output) override;

	//! 释放yolo
	void Release();



private:
	~YOLOV5() {};
};

