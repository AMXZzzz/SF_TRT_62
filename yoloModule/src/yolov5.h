#pragma once
#define API_EXPORT
#include "yolo.h"


class  YOLOV5 final : public YOLO {
public:
	//! �вι���
	YOLOV5(float* conf, float* iou, Process* process) : YOLO(conf, iou, process) {};

	//! ��ȡ��ǰ���ñ�����
	sf::Type::YoloType getYoloType() override;

	//! yolov5�ĺ���������yolov7
	void DecodeOutput(float* output) override;

	//! �ͷ�yolo
	void Release();



private:
	~YOLOV5() {};
};

