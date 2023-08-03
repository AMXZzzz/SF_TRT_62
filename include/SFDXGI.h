#pragma once
#include<opencv2/opencv.hpp>
#include<windows.h>
#include <dxgi1_5.h>
#include <dxgi.h>
#include <d3d11.h>


typedef enum SF_DXGI_ERROR {
	DXGI_ERROR = - 1,			/* 无类型错误*/
	DXGI_SUCCECC = 0,			/* 执行成功 */
	DXGI_DEVICE_ERROR = 1,		/* 初始化d3ddevice失败 */
	DXGI_DEVICE2_ERROR = 2,		/* 初始化d3ddevice2失败 */
	DXGI_ADAPTER_ERROR = 3,		/* 初始化d3d适配器失败 */
	DXGI_OUTPUT_ERROR = 4,		/* 初始化d3d输出接口失败 */
	DXGI_OUTPUT5_ERROR = 5,		/* 初始化d3d输出接口2失败 */
	DXGI_DUPLICATE_ERROR = 6,	/* 初始化d3d显示器信息失败 */
	DXGI_FACTORY_ERROR = 7,		/* 创建工厂模板失败 */

}SF_DXGI_ERROR;

struct SFPoint {
	int origin_x = 0;
	int origin_y = 0;
	int center_x = 0;
	int center_y = 0;
	int CapWidth = 0;
	int CapHeight = 0;
	int WinWidth = 0;
	int WinHeight = 0;
};

class SF_DXGI {
public:
	SF_DXGI() {};
	~SF_DXGI() {};
	static SF_DXGI& Get() {
		static SF_DXGI m_pInstance;
		return m_pInstance;
	};
	SF_DXGI_ERROR CaptureResource(
		int Width,
		int Height);

	BOOL BitmapToMat(
		cv::Mat* img);

	VOID Release();

	struct SFPoint point;

private:
	ID3D11Device* Device = nullptr;	//ID3D11 设备
	ID3D11DeviceContext* Context = nullptr;	//ID3D11 设备上下文
	IDXGIDevice2* Device2 = nullptr;
	IDXGIAdapter* Adapter = nullptr;
	IDXGIOutput* Output = nullptr;
	IDXGIOutput5* Output5 = nullptr;
	IDXGIOutputDuplication* Duplicate = nullptr;
	IDXGIFactory* Factory;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	D3D11_TEXTURE2D_DESC dataDesc = { NULL };

	UINT Monitor_idx = 0;
	cv::Rect rect;
	BOOL need_updata = TRUE;
	SF_DXGI_ERROR CreateInterface();
	SF_DXGI_ERROR GetAdapterInterface();
	VOID ObtainDesc(ID3D11Texture2D*);
};