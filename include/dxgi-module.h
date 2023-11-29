#pragma once
#include <opencv2/opencv.hpp>
#include <windows.h>

#define LIBEXP extern "C" __declspec(dllexport)

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


typedef enum SF_DXGI_ERROR {
	DXGI_SUCCECC = 0,			/* 执行成功 */
	DXGI_DEVICE_ERROR = 1,		/* 初始化d3ddevice失败 */
	DXGI_DEVICE2_ERROR = 2,		/* 初始化d3ddevice2失败 */
	DXGI_DUPLICATE_ERROR = 3,	/* 初始化d3d显示器信息失败 */
	DXGI_FACTORY_ERROR = 4,		/* 创建工厂模板失败 */
	DXGI_PICTURE_NONE = 5,		//! 画面无变化
	DXGI_REINITIALIZE = 6,		//! 重新初始化
	DXGI_ERROR_DESKTOP_TEXTURE = 7,		//! 获取桌面纹理错误
	DXGI_ERROR_CREATE_TEXTURE = 8,		//! 创建兼容纹理失败
	DXGI_ERROR_POINT_NULL = 9,		//! point指针无效

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



