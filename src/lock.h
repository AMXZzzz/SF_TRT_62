#pragma once
#include <vector>
#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include "parameter.h"

#ifndef API_EXPORT
#define SF_API extern "C" __declspec(dllimport)
#else
#define SF_API extern "C" __declspec(dllexport)
#endif

struct IRect {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

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

//! 需要填充的参数
struct LOCKINFO {

	//! 用于存储处理结果
	Process* process;
	IPoint* point;
	IParameter* parame;
	int lock_method = 0;
	std::shared_ptr<spdlog::logger> logger;
};

class Lock {
public:
	//! 初始化移动方式
	virtual bool initLock() = 0;

	//! 触发移动
	virtual void triggerMove() = 0;

	//! 释放
	virtual void Release() = 0;
	
	//! 
	Lock(LOCKINFO* info) : m_process(info->process), m_point(info->point), m_parame(info->parame) {};
	
	//! 基类析构 
	virtual ~Lock() {};

protected:
	//! 用于存储处理结果
	Process* m_process;
	IPoint* m_point;
	IParameter* m_parame;
	std::shared_ptr<spdlog::logger> m_logger;
};



namespace sf {
	SF_API Lock* crateLock(LOCKINFO* info);
}

void chooseClass(std::vector<float>& EuclideanDistance,
	std::vector<int>& indices,
	Process* process,
	IPoint* point,
	const int& idx,
	const bool& classes);
