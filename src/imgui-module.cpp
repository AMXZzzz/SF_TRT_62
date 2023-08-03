#include "imgui-module.h"

#define TRTHEAD "[IMGUI]: "
#define LOGINFO(format,...) if (_logger) {_logger->info(TRTHEAD ## format, __VA_ARGS__);} 
#define LOGWARN(format,...)  if (_logger) {_logger->warn(TRTHEAD ## format, __VA_ARGS__);}
#define LOGERROR(format,...) if (_logger) {_logger->error(TRTHEAD ## format, __VA_ARGS__);}

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//ImguiModule* imguimodule = &ImguiModule::Get();
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return TRUE;
	switch (msg) {
	case WM_SIZE:	// 如果窗口是固定大小则无需这些代码，
		//if (imguimodule->g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
		//	imguimodule->CleanupRenderBuffer();
		//	imguimodule->g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
		//	imguimodule->CreateRanderBuffer();
		//}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // 禁用 ALT 应用程序菜单
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void imguiUi::cleanupRenderBuffer() {
	if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = NULL;
	}
}

bool imguiUi::createImguiWinClass() {
	// 注册窗口类		Create winClass
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Temp Example", NULL };
	RegisterClassExW(&wc);
	hwnd = CreateWindow(wc.lpszClassName, L"SF_TRT_62", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	//初始化d3d		Init D3D
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK) {
		cleanupRenderBuffer();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
		return false;
	}
	return true;
}

IStates imguiUi::Init() {
	if (!createImguiWinClass()) {
		LOGWARN("创建Imgui窗口类失败");
		return IStates(State::UNKONEERR, "创建Imgui窗口类失败");
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();

	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// 启用键盘控制
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// 启用对接
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// 取消保存ini文件
	io->IniFilename = NULL;
	io->ConfigViewportsNoAutoMerge = TRUE;

	ImFont* font = io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 16.f, NULL, io->Fonts->GetGlyphRangesChineseFull());
	ImGuiStyle& style = ImGui::GetStyle();

	if (io->ConfigFlags == ImGuiConfigFlags_ViewportsEnable) {
		style.Colors[ImGuiCol_WindowBg].w = 10.0f;	// 颜色深度
		style.WindowRounding = 10.0f;				// 角弧度
	}

	// 初始化渲染后端		Initialize the rendering backend
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


	// 设置控件样式		Set control style
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);			// 控件弧度
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5);			// 边缘阴影
	ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 10.f);				// 滑条滑块弧度

	return IStates();
}

void imguiUi::SetNextFrame() {
	MSG msg{};
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 使用imgui渲染帧		Render frames using imgui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void imguiUi::PresentFrame() {
	// 绑定到管线		bind to the pipe
	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);	//获取管线
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);	// 初始化管线
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());	// 获取ImGui的控件Buff
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();	//绑定到管线
	}
	g_pSwapChain->Present(1, 0); // 展示到屏幕	Present
}

void imguiUi::Rendering() {

	SetNextFrame();
	ImGui::Begin(u8"SF_TRT_62《bilibili:随风而息》", &_parame->uiStop, ImGuiWindowFlags_NoSavedSettings + ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::SliderFloat(u8"conf",&_parame->conf, 0.01f, 1.0f);
	ImGui::SliderFloat(u8"iou",&_parame->iou, 0.01f, 1.0f);

	ImGui::RadioButton(u8"YOLOV5/7", &yolo_type, 0);
	ImGui::SameLine();
	ImGui::RadioButton(u8"YOLOV8", &yolo_type, 1);
	ImGui::SameLine();
	ImGui::RadioButton(u8"YOLOX", &yolo_type, 2);

	ImGui::RadioButton(u8"TRT", &backend, 0);
	ImGui::SameLine();
	ImGui::RadioButton(u8"DML", &backend, 1);

	ImGui::InputText(u8"模型路径##model_path", model_path, MAX_PATH);

	switch (_parame->ai_is_run) {
	case true:
		if (ImGui::Button(u8"停止")) {
			_parame->executionStatus = false;
		}
		break;
	case false:		
		if (ImGui::Button(u8"运行")) {
			_actuator->executionThread(yolo_type, backend, model_path);
		}
		break;
	}

	ImGui::End();
	PresentFrame();
}


