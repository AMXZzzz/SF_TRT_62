#pragma once
#include <vector>
#include <opencv2/core/types.hpp>

struct IPoint {
	int origin_x = 0;	//! ԭ��x
	int origin_y = 0;	//! ԭ��y
	int center_x = 0;	//! ���ĵ�x
	int center_y = 0;	//! ���ĵ�y
	int CapWidth = 0;	//! ��ͼ��Χ-��
	int CapHeight = 0;	//! ��ͼ��Χ-��
	int WinWidth = 0;	//! ��Ļ��
	int WinHeight = 0;	//! ��Ļ��
};

// !�м�����ṹ��
struct Process {
	std::vector<cv::Rect> boxes;	//! ����Ŀ�������
	std::vector<int> indices;		//! ����Ŀ�������
	std::vector<int> classes;		//! ����Ŀ�����
	std::vector<float> confidences;	//! ����Ŀ������Ŷ�

	int getClassName(int idx) {
		return classes[indices[idx]];
	}
};

