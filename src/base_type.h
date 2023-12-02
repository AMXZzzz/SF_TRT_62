#pragma once
#include <vector>
#include <opencv2/core/types.hpp>

struct IPoint {
	int origin_x = 0;	//! 原点x
	int origin_y = 0;	//! 原点y
	int center_x = 0;	//! 中心点x
	int center_y = 0;	//! 中心点y
	int CapWidth = 0;	//! 截图范围-宽
	int CapHeight = 0;	//! 截图范围-高
	int WinWidth = 0;	//! 屏幕宽
	int WinHeight = 0;	//! 屏幕高
};

// !中间先验结构体
struct Process {
	std::vector<cv::Rect> boxes;	//! 所有目标的坐标
	std::vector<int> indices;		//! 所有目标的索引
	std::vector<int> classes;		//! 所有目标的类
	std::vector<float> confidences;	//! 所有目标的置信度

	int getClassName(int idx) {
		return classes[indices[idx]];
	}
};

