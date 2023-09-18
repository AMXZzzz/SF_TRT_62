#pragma once
#define API_EXPORT
#include "yolo.h"

class YOLOX final :public YOLO {	
public:
	//! �������Ŷ�ָ��,iouָ��,����ṹ��
	YOLOX(float* conf, float* iou, Process* process) : YOLO(conf, iou, process) {};

	//! ��ȡ��һ����ֵ
	const float getNormalized() override;

	//! ��ȡ���ñ�����
	sf::Type::YoloType getYoloType() override;

	//! ������д
	void DecodeOutput(float* output) override;

	//! �����ͷ�
	void Release();

private:
	//! yolox����㺯��
	void generate_grids_and_stride();

	~YOLOX() {};

	struct GridAndStride {				// ����ά��
		int gh;
		int gw;
		int stride;
	};

	std::vector<GridAndStride> grid_strides;	// ����
	float Normalized = 1.f;

};

