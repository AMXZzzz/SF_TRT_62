#pragma once
#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include <parameter.h>
#include <dxgi-module.h>

#define OPENCV_WINDOWS_NAME "SF_62"


class Intermediary {
public:
	bool InitSharedParame();		//�� ��ʼ�������ڴ�
	bool InitLogger();				//�� ��ʼ����־
	bool InitYoloTable();			//�� ��ʼ��yolo
	bool InitFrame();				//�� ��ʼ��frame
	bool InitModel();				//�� ��ʼ��ģ��
	bool InitLock();				//!  ��ʼ������
	bool InitDX();					//�� ��ʼ��DX��ͼ
	bool Detection();				//�� ���
	void DrawBox(cv::Mat& img);		//�� ����
	std::shared_ptr<spdlog::logger> getLogger();	//�� ��ȡ��־ָ��
	void Release();					//�� �ͷ��ڴ�
private:
	std::shared_ptr<spdlog::logger> g_logger;	//����־����ָ��
	Process process;							//����������ָ��
	Point point;								//! ��ͼ����
	Parameter* parme;							//�������ڴ�ָ��
	//! ��Ҫ�ͷŵ�ָ��
	YOLO* yolo;									//��yoloָ��		
	Frame* frame;								//��frameָ��	
	DXGI* dxgi;									//��dxgiָ��	
};

