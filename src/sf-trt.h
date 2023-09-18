#pragma once
#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include <dxgi-module.h>
#include "frame.h"
#include "lock.h"

#define OPENCV_WINDOWS_NAME "SF_62"

class Intermediary {
public:
	bool InitSharedParame();		//！ 初始化共享内存
	bool InitLogger();				//！ 初始化日志
	bool InitYoloTable();			//！ 初始化yolo
	bool InitFrame();				//！ 初始化frame
	bool InitModel();				//！ 初始化模型
	bool InitLock();				//!  初始化自瞄
	bool InitDX();					//！ 初始化DX截图
	bool Detection();				//！ 检测
	void DrawBox(cv::Mat& img);		//！ 绘制
	std::shared_ptr<spdlog::logger> getLogger();	//！ 获取日志指针
	void Release();					//！ 释放内存
private:
	std::shared_ptr<spdlog::logger> g_logger;	//！日志智能指针
	Process process;							//！处理容器指针
	IPoint point;								//! 截图参数
	IParameter* parame;							//！共享内存指针
	//! 需要释放的指针
	YOLO* yolo;									//！yolo指针		
	Frame* frame;								//！frame指针	
	DXGI* dxgi;									//！dxgi指针	
	Lock* lock;									//! 移动逻辑
};

