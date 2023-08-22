#include "imgui-module.h"
#include "trt-module.h"
#include "control-algorithm.h"
#include "parameter.h"


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
	case WM_SIZE:
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


static void ExplanationMake(const char* strdata, bool same = true) {
	if (same) ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 25.f);
		ImGui::TextUnformatted(strdata);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static void ICheckbox(const char* label, bool* v) {
	ImGui::Checkbox(label, v);
}

static void IFovPixelInput(const char* label, int* val) {
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	ImGui::InputScalar(label, ImGuiDataType_S32, val, NULL, NULL, "%d", NULL);
}

static void IText(const char* label) {
	ImGui::Text(label);
}

static void IPushID(const char* label) {
	ImGui::PushID(label);
}

static void IRadioButton(const char* label, int* key, int val) {
	ImGui::SameLine();
	ImGui::RadioButton(label, key, val);
}

static void IInputScalar(const char* label, void* p_data) {
	ImGui::InputScalar(label, ImGuiDataType_S32, p_data, NULL, NULL, "%d", NULL);
}

static bool IButton(const char* label) {
	return ImGui::Button(label);
}

static inline void ClassChoose(int& num, const char* label, bool* v) {
	if (num) {
		ImGui::SameLine();
		ImGui::Checkbox(label, v);
		num--;
	}
}

void imguiUi::cleanupRenderBuffer() {
	if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = NULL;
	}
}

static void ISliderFloat(const char* label, float* v, float v_min, float v_max) {
	ImGui::SliderFloat(label, v, v_min, v_max);
}

static void ISliderInt(const char* label, int* v, int v_min, int v_max) {
	ImGui::SliderInt(label, v, v_min, v_max);
}

static VOID IText2(int loop, int cap, int pred, int fps) {
	ImGui::Text( u8"����:[ѭ��:%-3d ��ͼ:%-3d ����:%-3d FPS:%-4d]",loop, cap, pred, fps);
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

void  imguiUi::GetParame() {
	ConfigModule* config = &ConfigModule::Get();
	config-> GetParame((char*)GetIniPath().c_str(),_parame);
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
	ImGui::Begin(u8"Smart Monitor", &_parame->uiStop, ImGuiWindowFlags_NoSavedSettings + ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Separator();
	ImGui::Separator();
	if (IButton(u8"����config")) {
		FileBrowser::Get().config_dialog.Open();
	}
	ExplanationMake(u8"��������һ��config�ļ�");
	ImGui::SameLine();
	if (IButton(u8"�������")) {
		ConfigModule::Get().SaveParame(_parame);
	}
	ExplanationMake(u8"���������Config.ini");

	ImGui::SetNextItemWidth(150);
	ImGui::InputTextWithHint(u8"Config�ļ���", u8"ʾ��:Config.ini", ConfigModule::Get().Config_name, 20);
	ExplanationMake(u8"���浽ָ����ini�ļ���Ĭ��ΪConfig.ini,ASCII����");


	ImGui::SliderFloat(u8"���Ŷ�", &_parame->conf, 0.01f, 1.0f);
	ISliderFloat(u8"λ��", &_parame->location, 0.f, 1.f);
	ImGui::SetNextItemWidth(60);
	IInputScalar(u8"������ؾ���", &_parame->max_pixels);

	if (!_parame->ai_is_run) {
		ImGui::RadioButton(u8"YOLOV5/7", &_parame->yolo_type, 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"YOLOV8", &_parame->yolo_type, 1);
		ImGui::SameLine();
		ImGui::RadioButton(u8"YOLOX", &_parame->yolo_type, 2);

		ImGui::RadioButton(u8"TRT", &_parame->backend, 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"DML", &_parame->backend, 1);

		ImGui::InputText(u8"ģ��·��##model_path", _parame->model_path, MAX_PATH);

		// way of moving
		ImGui::RadioButton(u8"GHUB", &_parame->move_way, 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"SendInput", &_parame->move_way, 1);
		ImGui::SameLine();
		ImGui::RadioButton(u8"�Զ���##�ƶ���ʽ", &_parame->move_way, 2);
		if (ImGui::Button(u8"����")) {
			_actuator->executionThread(_parame->yolo_type, _parame->backend, _parame->model_path);
		}
	}
	else {
		ISliderInt(u8"���Χ", &_parame->effectiverange, 0, _parame->max_range);
		ICheckbox(u8"��ʾ������", &_parame->showWindow);
		// show performance
		IText2(5, 1, 2, 200); 
		if (ImGui::Button(u8"ֹͣ")) {
			_parame->executionStatus = false;
		}
	}
	// ���ѡ��
	ICheckbox(u8"ģ�����", &_parame->ModelClasses);
	if (_parame->ModelClasses) {
		int class_num_temp = _parame->class_number;
		ClassChoose(class_num_temp, u8"0", &_parame->class0);
		ClassChoose(class_num_temp, u8"1", &_parame->class1);
		ClassChoose(class_num_temp, u8"2", &_parame->class2);
		ClassChoose(class_num_temp, u8"3", &_parame->class3);
	}

	ImGui::Separator();
	ImGui::Separator();
	ImGui::Text(u8"�� ");
	ImGui::SameLine();
	ICheckbox(u8"��������", &_parame->key_ui);
	if (_parame->key_ui) {
		if (_parame->AimSwitch == TRUE) {
			IPushID(u8"״̬:ON ");
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.f / 255, 255.f / 255, 0.0f / 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.f / 255, 225.f / 255, 0.0f / 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.f / 255, 155.f / 255, 0.0f / 255));
			if (IButton(u8"״̬:ON "))
				_parame->AimSwitch = FALSE;
			ImGui::PopStyleColor(3);
			ImGui::PopID();

		}
		else {
			IPushID(u8"״̬:OFF");
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(255.f / 255, 0.0f / 255, 0.0f / 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(225.f / 255, 0.0f / 255, 0.0f / 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(155.f / 255, 0.0f / 255, 0.0f / 255));
			if (IButton(u8"״̬:OFF"))
				_parame->AimSwitch = TRUE;
			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}

		IRadioButton(u8"��", &_parame->lock_key, 0x01);
		IRadioButton(u8"��", &_parame->lock_key, 0x02);
		IRadioButton(u8"��", &_parame->lock_key, 0x05);

		ImGui::SetNextItemWidth(30);	//���ÿ��
		ImGui::SameLine();
		IInputScalar(u8"�Զ���##����2", &_parame->lock_key2);
		ExplanationMake(u8"�Զ��崥������,��Ӧ��������ʮ������,0Ϊ��ʹ��");

		ICheckbox(u8"���", &_parame->auto_fire);
		if (_parame->auto_fire == TRUE) {
			ImGui::SetNextItemWidth(40);	//���ÿ��
			IInputScalar(u8"���", &_parame->auto_interval);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(40);	//���ÿ��
			IInputScalar(u8"���", &_parame->auto_random);

			IRadioButton(u8"��ʽ1", &_parame->auto_model, 0);
			IRadioButton(u8"��ʽ2", &_parame->auto_model, 1);
			ExplanationMake(u8"��ʽ1:���ƶ���Ŀ�귶Χ���ٿ�ǹ����ʽ2:���ƶ�,��Ŀ�귶Χ�ڿ�ǹ");
		}
	}

	ImGui::Separator();
	ImGui::Separator();
	ImGui::Text(u8"�� ");
	ImGui::SameLine();
	ICheckbox(u8"PID����", &_parame->pid_ui);
	if (_parame->pid_ui) {
		IText(u8"��	X��");
		ImGui::SliderFloat(u8"P##P_X", &_parame->P_x, 0.f, 1.f);
		ImGui::SliderFloat(u8"I##I_X", &_parame->I_x, 0.f, 1.f);
		ImGui::SliderFloat(u8"P##D_X", &_parame->D_x, 0.f, 1.f);

		IText(u8"��	Y��");
		ImGui::SliderFloat(u8"P##P_Y", &_parame->P_y, 0.f, 1.f);
		ImGui::SliderFloat(u8"I##I_Y", &_parame->I_y, 0.f, 1.f);
		ImGui::SliderFloat(u8"P##D_Y", &_parame->D_y, 0.f, 1.f);
	}

	ImGui::Separator();
	ImGui::Separator();
	ImGui::Text(u8"�� ");
	ImGui::SameLine();
	ICheckbox(u8"FOV�㷨", &_parame->fov_algorithm);
	if (_parame->fov_algorithm) {
		// &&&ռλ��
		if (_parame->fov_algorithm) {
			IFovPixelInput(u8"X������", &_parame->game_x_pixel);
			ImGui::SameLine();
			IFovPixelInput(u8"Y������", &_parame->game_y_pixel);
		}
	}

	ImGui::End();
	PresentFrame();
}






