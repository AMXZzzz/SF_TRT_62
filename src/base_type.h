#pragma once
#include <vector>
#include <opencv2/core/types.hpp>


// !�м�����ṹ��
struct Process {
	std::vector<cv::Rect> boxes;
	std::vector<int> indices;
	std::vector<int> classes;
	std::vector<float> confidences;
};