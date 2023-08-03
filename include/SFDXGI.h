#pragma once
#include<opencv2/opencv.hpp>
#include<windows.h>
#include <dxgi1_5.h>
#include <dxgi.h>
#include <d3d11.h>


typedef enum SF_DXGI_ERROR {
	DXGI_ERROR = - 1,			/* �����ʹ���*/
	DXGI_SUCCECC = 0,			/* ִ�гɹ� */
	DXGI_DEVICE_ERROR = 1,		/* ��ʼ��d3ddeviceʧ�� */
	DXGI_DEVICE2_ERROR = 2,		/* ��ʼ��d3ddevice2ʧ�� */
	DXGI_ADAPTER_ERROR = 3,		/* ��ʼ��d3d������ʧ�� */
	DXGI_OUTPUT_ERROR = 4,		/* ��ʼ��d3d����ӿ�ʧ�� */
	DXGI_OUTPUT5_ERROR = 5,		/* ��ʼ��d3d����ӿ�2ʧ�� */
	DXGI_DUPLICATE_ERROR = 6,	/* ��ʼ��d3d��ʾ����Ϣʧ�� */
	DXGI_FACTORY_ERROR = 7,		/* ��������ģ��ʧ�� */

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
	ID3D11Device* Device = nullptr;	//ID3D11 �豸
	ID3D11DeviceContext* Context = nullptr;	//ID3D11 �豸������
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