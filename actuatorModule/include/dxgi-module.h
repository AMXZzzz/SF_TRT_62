#pragma once
#include<opencv2/opencv.hpp>
#include <windows.h>

#define LIBEXP extern "C" __declspec(dllexport)

typedef enum SF_DXGI_ERROR {
	DXGI_ERROR = -1,			/* �����ʹ���*/
	DXGI_SUCCECC = 0,			/* ִ�гɹ� */
	DXGI_DEVICE_ERROR = 1,		/* ��ʼ��d3ddeviceʧ�� */
	DXGI_DEVICE2_ERROR = 2,		/* ��ʼ��d3ddevice2ʧ�� */
	DXGI_ADAPTER_ERROR = 3,		/* ��ʼ��d3d������ʧ�� */
	DXGI_OUTPUT_ERROR = 4,		/* ��ʼ��d3d����ӿ�ʧ�� */
	DXGI_OUTPUT5_ERROR = 5,		/* ��ʼ��d3d����ӿ�2ʧ�� */
	DXGI_DUPLICATE_ERROR = 6,	/* ��ʼ��d3d��ʾ����Ϣʧ�� */
	DXGI_FACTORY_ERROR = 7,		/* ��������ģ��ʧ�� */

}SF_DXGI_ERROR;

struct Point {
	int origin_x = 0;	//! ԭ��x
	int origin_y = 0;	//! ԭ��y
	int center_x = 0;	//! ���ĵ�x
	int center_y = 0;	//! ���ĵ�y
	int CapWidth = 0;	//! ��ͼ��Χ-��
	int CapHeight = 0;	//! ��ͼ��Χ-��
	int WinWidth = 0;	//! ��Ļ��
	int WinHeight = 0;	//! ��Ļ��
};

class DXGI {
public:
	virtual void DXGIInitPont(Point* point) = 0;

	virtual SF_DXGI_ERROR CaptureResource(int,int) = 0;

	virtual bool BitmapToMat(cv::Mat* img) = 0;
	
	virtual void Release() = 0;

	virtual ~DXGI() {};
private:

};

namespace sf {
	LIBEXP DXGI* WINAPI createDxgi();
}