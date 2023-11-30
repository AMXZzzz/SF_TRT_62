#pragma once
#include <opencv2/opencv.hpp>
#include <windows.h>

#define LIBEXP extern "C" __declspec(dllexport)

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


typedef enum SF_DXGI_ERROR {
	DXGI_SUCCECC = 0,			/* ִ�гɹ� */
	DXGI_DEVICE_ERROR = 1,		/* ��ʼ��d3ddeviceʧ�� */
	DXGI_DEVICE2_ERROR = 2,		/* ��ʼ��d3ddevice2ʧ�� */
	DXGI_DUPLICATE_ERROR = 3,	/* ��ʼ��d3d��ʾ����Ϣʧ�� */
	DXGI_FACTORY_ERROR = 4,		/* ��������ģ��ʧ�� */
	DXGI_PICTURE_NONE = 5,		//! �����ޱ仯
	DXGI_REINITIALIZE = 6,		//! ���³�ʼ��
	DXGI_ERROR_DESKTOP_TEXTURE = 7,		//! ��ȡ�����������
	DXGI_ERROR_CREATE_TEXTURE = 8,		//! ������������ʧ��
	DXGI_ERROR_POINT_NULL = 9,		//! pointָ����Ч

}SF_DXGI_ERROR;

class DXGI {
public:
	virtual SF_DXGI_ERROR SetCaptureResource(int, int) = 0;
	virtual SF_DXGI_ERROR GetBitmapToMat(cv::Mat* img) = 0;
	virtual void Release() = 0;
	virtual ~DXGI() {};

};

namespace sf {
	DXGI* createDxgi(IPoint* point);
}



