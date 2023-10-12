#pragma once
#define API_EXPORT
#include "yolo.h"

class _declspec(dllexport) YOLOV8 final : public YOLO {	// ��(��һ��)��ģ��
public:


	YOLOV8(float* conf, float* iou, Process* process) :YOLO(conf, iou, process) {};
	//! �ͷ�
	void Release() {
		delete this;
	}

	//! ��ȡ����ڵ������
	//! ����ָ��
	const char** getOutputName() override;

	//! ��ȡ���ñ������
	sf::Type::YoloType getYoloType() override;


	//! ��֤ά���Ƿ���ȷ
	bool dims_error(int64_t dim1, int64_t dim2) override;

	//! ����
	void DecodeOutput(float* output) override;

private:
	~YOLOV8() {};

	// YOLOV8�Լ��Ĳ���
	const char* output_name[1] = { "output0" };
};
