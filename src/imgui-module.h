#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_internal.h>
#include <imgui/imstb_rectpack.h>
#include <imgui/imstb_truetype.h>
#include <imgui/imstb_truetype.h>
#include <imgui/imfilebrowser.h>

#include "ui-base.h"

class imguiUi : public uiBase {
public:
	virtual  IStates Init() override;

	virtual  void Rendering() override;

	imguiUi(Actuator* actuator, Parameter* parame, std::shared_ptr<spdlog::logger> logger) : uiBase(actuator, parame, logger) {}

private:

	ImGuiIO* io;
	HWND hwnd = NULL;
	ID3D11Device* g_pd3dDevice = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;
	ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
	const float clear_color_with_alpha[4] = { 0.45f, 0.55f, 0.60f, 1.00f };

	bool createImguiWinClass();
	void cleanupRenderBuffer();
	void SetNextFrame();
	void PresentFrame();
};




