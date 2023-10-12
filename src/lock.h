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
	int origin_x = 0;	//! ԭ��x
	int origin_y = 0;	//! ԭ��y
	int center_x = 0;	//! ���ĵ�x
	int center_y = 0;	//! ���ĵ�y
	int CapWidth = 0;	//! ��ͼ��Χ-��
	int CapHeight = 0;	//! ��ͼ��Χ-��
	int WinWidth = 0;	//! ��Ļ��
	int WinHeight = 0;	//! ��Ļ��
};

//! ��Ҫ���Ĳ���
struct LOCKINFO {

	//! ���ڴ洢������
	Process* process;
	IPoint* point;
	IParameter* parame;
	int lock_method = 0;
	std::shared_ptr<spdlog::logger> logger;
};

class Lock {
public:
	//! ��ʼ���ƶ���ʽ
	virtual bool initLock() = 0;

	//! �����ƶ�
	virtual void triggerMove() = 0;

	//! �ͷ�
	virtual void Release() = 0;
	
	//! 
	Lock(LOCKINFO* info) : m_process(info->process), m_point(info->point), m_parame(info->parame) {};
	
	//! �������� 
	virtual ~Lock() {};

protected:
	//! ���ڴ洢������
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
