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
	case WM_SIZE:	// ��������ǹ̶���С��������Щ���룬
		//if (imguimodule->g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
		//	imguimodule->CleanupRenderBuffer();
		//	imguimodule->g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
		//	imguimodule->CreateRanderBuffer();
		//}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // ���� ALT Ӧ�ó���˵�
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
	// ע�ᴰ����		Create winClass
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Temp Example", NULL };
	RegisterClassExW(&wc);
	hwnd = CreateWindow(wc.lpszClassName, L"SF_TRT_62", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	//��ʼ��d3d		Init D3D
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
		LOGWARN("����Imgui������ʧ��");
		return IStates(State::UNKONEERR, "����Imgui������ʧ��");
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();

	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// ���ü��̿���
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// ���öԽ�
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// ȡ������ini�ļ�
	io->IniFilename = NULL;
	io->ConfigViewportsNoAutoMerge = TRUE;

	ImFont* font = io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 16.f, NULL, io->Fonts->GetGlyphRangesChineseFull());
	ImGuiStyle& style = ImGui::GetStyle();

	if (io->ConfigFlags == ImGuiConfigFlags_ViewportsEnable) {
		style.Colors[ImGuiCol_WindowBg].w = 10.0f;	// ��ɫ���
		style.WindowRounding = 10.0f;				// �ǻ���
	}

	// ��ʼ����Ⱦ���		Initialize the rendering backend
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


	// ���ÿؼ���ʽ		Set control style
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);			// �ؼ�����
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5);			// ��Ե��Ӱ
	ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 10.f);				// �������黡��

	return IStates();
}

void imguiUi::SetNextFrame() {
	MSG msg{};
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// ʹ��imgui��Ⱦ֡		Render frames using imgui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void imguiUi::PresentFrame() {
	// �󶨵�����		bind to the pipe
	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);	//��ȡ����
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);	// ��ʼ������
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());	// ��ȡImGui�Ŀؼ�Buff
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();	//�󶨵�����
	}
	g_pSwapChain->Present(1, 0); // չʾ����Ļ	Present
}

void imguiUi::Rendering() {

	SetNextFrame();
	ImGui::Begin(u8"SF_TRT_62��bilibili:����Ϣ��", &_parame->uiStop, ImGuiWindowFlags_NoSavedSettings + ImGuiWindowFlags_AlwaysAutoResize);

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

	ImGui::InputText(u8"ģ��·��##model_path", model_path, MAX_PATH);

	switch (_parame->ai_is_run) {
	case true:
		if (ImGui::Button(u8"ֹͣ")) {
			_parame->executionStatus = false;
		}
		break;
	case false:		
		if (ImGui::Button(u8"����")) {
			_actuator->executionThread(yolo_type, backend, model_path);
		}
		break;
	}

	ImGui::End();
	PresentFrame();
}


