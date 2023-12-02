#pragma once
#include <vector>
#include <opencv2/core/types.hpp>


// !中间先验结构体
struct Process {
	std::vector<cv::Rect> boxes;
	std::vector<int> indices;
	std::vector<int> classes;
	std::vector<float> confidences;
};