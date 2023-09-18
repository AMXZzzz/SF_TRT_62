#pragma once
#define API_EXPORT
#include "yolo.h"

class _declspec(dllexport) YOLOV8 final : public YOLO {	// 底(第一声)层模块
public:


	YOLOV8(float* conf, float* iou, Process* process) :YOLO(conf, iou, process) {};
	//! 释放
	void Release() {
		delete this;
	}

	//! 获取输出节点的名称
	//! 二级指针
	const char** getOutputName() override;

	//! 获取配置表的类型
	sf::Type::YoloType getYoloType() override;


	//! 验证维度是否正确
	bool dims_error(int64_t dim1, int64_t dim2) override;

	//! 解码
	void DecodeOutput(float* output) override;

private:
	~YOLOV8() {};

	// YOLOV8自己的层名
	const char* output_name[1] = { "output0" };
};
