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
		if ((wParam & 0xfff0) == SC_KEYMENU) // 禁用 ALT 应用程序菜单
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
	ImGui::Text( u8"性能:[循环:%-3d 截图:%-3d 推理:%-3d FPS:%-4d]",loop, cap, pred, fps);
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
	ImGui::Begin(u8"Smart Monitor", &_parame->uiStop, ImGuiWindowFlags_NoSavedSettings + ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Separator();
	ImGui::Separator();
	if (IButton(u8"加载config")) {
		FileBrowser::Get().config_dialog.Open();
	}
	ExplanationMake(u8"启动加载一次config文件");
	ImGui::SameLine();
	if (IButton(u8"保存参数")) {
		ConfigModule::Get().SaveParame(_parame);
	}
	ExplanationMake(u8"保存参数到Config.ini");

	ImGui::SetNextItemWidth(150);
	ImGui::InputTextWithHint(u8"Config文件名", u8"示例:Config.ini", ConfigModule::Get().Config_name, 20);
	ExplanationMake(u8"保存到指定的ini文件，默认为Config.ini,ASCII编码");


	ImGui::SliderFloat(u8"置信度", &_parame->conf, 0.01f, 1.0f);
	ISliderFloat(u8"位置", &_parame->location, 0.f, 1.f);
	ImGui::SetNextItemWidth(60);
	IInputScalar(u8"最大像素距离", &_parame->max_pixels);

	if (!_parame->ai_is_run) {
		ImGui::RadioButton(u8"YOLOV5/7", &_parame->yolo_type, 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"YOLOV8", &_parame->yolo_type, 1);
		ImGui::SameLine();
		ImGui::RadioButton(u8"YOLOX", &_parame->yolo_type, 2);

		ImGui::RadioButton(u8"TRT", &_parame->backend, 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"DML", &_parame->backend, 1);

		ImGui::InputText(u8"模型路径##model_path", _parame->model_path, MAX_PATH);

		// way of moving
		ImGui::RadioButton(u8"GHUB", &_parame->move_way, 0);
		ImGui::SameLine();
		ImGui::RadioButton(u8"SendInput", &_parame->move_way, 1);
		ImGui::SameLine();
		ImGui::RadioButton(u8"自定义##移动方式", &_parame->move_way, 2);
		if (ImGui::Button(u8"运行")) {
			_actuator->executionThread(_parame->yolo_type, _parame->backend, _parame->model_path);
		}
	}
	else {
		ISliderInt(u8"最大范围", &_parame->effectiverange, 0, _parame->max_range);
		ICheckbox(u8"显示推理窗口", &_parame->showWindow);
		// show performance
		IText2(5, 1, 2, 200); 
		if (ImGui::Button(u8"停止")) {
			_parame->executionStatus = false;
		}
	}
	// 类别选择
	ICheckbox(u8"模型类别", &_parame->ModelClasses);
	if (_parame->ModelClasses) {
		int class_num_temp = _parame->class_number;
		ClassChoose(class_num_temp, u8"0", &_parame->class0);
		ClassChoose(class_num_temp, u8"1", &_parame->class1);
		ClassChoose(class_num_temp, u8"2", &_parame->class2);
		ClassChoose(class_num_temp, u8"3", &_parame->class3);
	}

	ImGui::Separator();
	ImGui::Separator();
	ImGui::Text(u8"※ ");
	ImGui::SameLine();
	ICheckbox(u8"按键设置", &_parame->key_ui);
	if (_parame->key_ui) {
		if (_parame->AimSwitch == TRUE) {
			IPushID(u8"状态:ON ");
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.f / 255, 255.f / 255, 0.0f / 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.f / 255, 225.f / 255, 0.0f / 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.f / 255, 155.f / 255, 0.0f / 255));
			if (IButton(u8"状态:ON "))
				_parame->AimSwitch = FALSE;
			ImGui::PopStyleColor(3);
			ImGui::PopID();

		}
		else {
			IPushID(u8"状态:OFF");
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(255.f / 255, 0.0f / 255, 0.0f / 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(225.f / 255, 0.0f / 255, 0.0f / 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(155.f / 255, 0.0f / 255, 0.0f / 255));
			if (IButton(u8"状态:OFF"))
				_parame->AimSwitch = TRUE;
			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}

		IRadioButton(u8"左", &_parame->lock_key, 0x01);
		IRadioButton(u8"右", &_parame->lock_key, 0x02);
		IRadioButton(u8"侧", &_parame->lock_key, 0x05);

		ImGui::SetNextItemWidth(30);	//设置宽度
		ImGui::SameLine();
		IInputScalar(u8"自定义##按键2", &_parame->lock_key2);
		ExplanationMake(u8"自定义触发按键,对应虚拟键表的十进制数,0为不使用");

		ICheckbox(u8"扳机", &_parame->auto_fire);
		if (_parame->auto_fire == TRUE) {
			ImGui::SetNextItemWidth(40);	//设置宽度
			IInputScalar(u8"间隔", &_parame->auto_interval);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(40);	//设置宽度
			IInputScalar(u8"随机", &_parame->auto_random);

			IRadioButton(u8"方式1", &_parame->auto_model, 0);
			IRadioButton(u8"方式2", &_parame->auto_model, 1);
			ExplanationMake(u8"方式1:先移动至目标范围内再开枪，方式2:不移动,在目标范围内开枪");
		}
	}

	ImGui::Separator();
	ImGui::Separator();
	ImGui::Text(u8"※ ");
	ImGui::SameLine();
	ICheckbox(u8"PID控制", &_parame->pid_ui);
	if (_parame->pid_ui) {
		IText(u8"·	X轴");
		ImGui::SliderFloat(u8"P##P_X", &_parame->P_x, 0.f, 1.f);
		ImGui::SliderFloat(u8"I##I_X", &_parame->I_x, 0.f, 1.f);
		ImGui::SliderFloat(u8"P##D_X", &_parame->D_x, 0.f, 1.f);

		IText(u8"·	Y轴");
		ImGui::SliderFloat(u8"P##P_Y", &_parame->P_y, 0.f, 1.f);
		ImGui::SliderFloat(u8"I##I_Y", &_parame->I_y, 0.f, 1.f);
		ImGui::SliderFloat(u8"P##D_Y", &_parame->D_y, 0.f, 1.f);
	}

	ImGui::Separator();
	ImGui::Separator();
	ImGui::Text(u8"※ ");
	ImGui::SameLine();
	ICheckbox(u8"FOV算法", &_parame->fov_algorithm);
	if (_parame->fov_algorithm) {
		// &&&占位符
		if (_parame->fov_algorithm) {
			IFovPixelInput(u8"X轴像素", &_parame->game_x_pixel);
			ImGui::SameLine();
			IFovPixelInput(u8"Y轴像素", &_parame->game_y_pixel);
		}
	}

	ImGui::End();
	PresentFrame();
}






