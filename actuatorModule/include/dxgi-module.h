#pragma once
#include<opencv2/opencv.hpp>
#include <windows.h>

#define LIBEXP extern "C" __declspec(dllexport)

typedef enum SF_DXGI_ERROR {
	DXGI_ERROR = -1,			/* 无类型错误*/
	DXGI_SUCCECC = 0,			/* 执行成功 */
	DXGI_DEVICE_ERROR = 1,		/* 初始化d3ddevice失败 */
	DXGI_DEVICE2_ERROR = 2,		/* 初始化d3ddevice2失败 */
	DXGI_ADAPTER_ERROR = 3,		/* 初始化d3d适配器失败 */
	DXGI_OUTPUT_ERROR = 4,		/* 初始化d3d输出接口失败 */
	DXGI_OUTPUT5_ERROR = 5,		/* 初始化d3d输出接口2失败 */
	DXGI_DUPLICATE_ERROR = 6,	/* 初始化d3d显示器信息失败 */
	DXGI_FACTORY_ERROR = 7,		/* 创建工厂模板失败 */

}SF_DXGI_ERROR;

struct Point {
	int origin_x = 0;	//! 原点x
	int origin_y = 0;	//! 原点y
	int center_x = 0;	//! 中心点x
	int center_y = 0;	//! 中心点y
	int CapWidth = 0;	//! 截图范围-宽
	int CapHeight = 0;	//! 截图范围-高
	int WinWidth = 0;	//! 屏幕宽
	int WinHeight = 0;	//! 屏幕高
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