#include <iostream>
#include <atltime.h>
#include "sf-trt.h"

#define EXPORT extern "C" __declspec(dllexport)

// 宏重载
#define TRTHEAD "[DLL]: "
#define LOGINFO(format,...) if (getLogger()) {getLogger()->info(TRTHEAD ## format, __VA_ARGS__);} 
#define LOGWARN(format,...)  if (getLogger()) {getLogger()->warn(TRTHEAD ## format, __VA_ARGS__);}
#define LOGERROR(format,...) if (getLogger()) {getLogger()->error(TRTHEAD ## format, __VA_ARGS__);}

// bool start_thread(bool is_local);


sf::Type::YoloType convertYoloType(int type) {
	switch (type) {
	case 0: return sf::Type::YoloType::TYPE_YOLOV5;
	case 1: return sf::Type::YoloType::TYPE_YOLOV8;
	case 2: return sf::Type::YoloType::TYPE_YOLOX;
	}
}

sf::Type::FrameType convertBackend(int type) {
	switch (type) {
	case 0: return sf::Type::FrameType::TRT_FRAME;
	case 1: return sf::Type::FrameType::DML_FRAME;
	}
}

//! 初始化共享内存
bool Intermediary::InitSharedParame() {

	HANDLE MapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SIZE, MapFileName);
	if (MapFile == NULL) {
		return false;
	}

	parame = (IParameter*)MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE);
	if (parame == NULL) {
		return false;
	}

	//! ------------- 仅做测试 -------------
	parame->conf = 0.3;
	parame->iou = 0.1;
	parame->showWindows = true;
	parame->module_path = "clbq_yolov5s_1w_640.engine";
	parame->yolo_type = 0;
	parame->backend = 0;
	parame->equipment = 0;

	return true;
}

bool Intermediary::InitLogger() {
	// 初始化日志
	CTime t = CTime::GetCurrentTime();
	static char temp[MAX_PATH]{};
	_snprintf_s(temp, MAX_PATH, "logs/%d-%d-%d %d-%d-%d.txt", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
	g_logger = spdlog::basic_logger_mt("SF_TRT", temp);
	g_logger->flush_on(spdlog::level::trace);

	return true;
}

//! 初始化yolo
bool Intermediary::InitYoloTable() {
	//! 检查指针
	if (&parame->conf == nullptr || &parame->iou == nullptr) {
		LOGINFO("参数指针为nullptr")
		return false;
	}
	YOLOINFO info{};
	info.type = convertYoloType(parame->yolo_type);
	info.conf = &parame->conf;
	info.iou = &parame->iou;
	info.process = &process;
	yolo = sf::createYoloTable(&info);
	return true;
}

//! 初始化frame
bool Intermediary::InitFrame() {
	//! 检查指针
	if (yolo == nullptr) {
		LOGWARN("yolo指针为nullptr")
		return false;
	}
	FRAMEINFO frame_info{};
	frame_info.frame_type = convertBackend(parame->backend);
	frame_info.yolo = yolo;
	frame_info.logger = getLogger();
	frame_info.equipment = parame->equipment;
	frame = sf::CreateFrame(&frame_info);
	return true;
}

//! 初始化模型
bool Intermediary::InitModel() {
	if (!frame->AnalyticalModel(parame->module_path)) {
		std::cout << frame->getLastErrorInfo().getErrorInfo() << std::endl;
		LOGWARN("加载模型错误:{}", frame->getLastErrorInfo().getErrorInfo().c_str())
		return false;
	}
	return true;
}

//! 初始化逻辑
bool Intermediary::InitLock() {
	LOCKINFO lock_info{};
	lock_info.parame = parame;
	lock_info.process = &process;
	lock_info.point = &point;
	lock_info.logger = getLogger();
	lock = sf::crateLock(&lock_info);

	if (!lock->initLock()) {
		LOGWARN("初始化移动逻辑失败");
		return false;
	}

	return true;
}

//! 初始化DX截图
bool Intermediary::InitDX() {
	dxgi = sf::createDxgi();
	if (dxgi == nullptr) {
		return false;
	}
	//! 初始化截图信息
	dxgi->DXGIInitPont(&point);
	//! 初始化DX截图范围
	dxgi->CaptureResource(yolo->getImageSize().width, yolo->getImageSize().height);
	return true;
}

//! 推理
bool Intermediary::Detection() {
	cv::Mat img;
	//! 置位信号
	parame->aiStatus = true;		//! ai运行状态

	while (!parame->uiSendStop) {
		//! 截图
		dxgi->BitmapToMat(&img);
		//! 推理
		frame->Detect(img);
		//! 自瞄
		lock->triggerMove();
		//! 绘制
		DrawBox(img);
	}

	//! 复位信号
	parame->aiStatus = false;

	//! 关闭窗口
	if (cv::getWindowProperty(OPENCV_WINDOWS_NAME, cv::WND_PROP_VISIBLE))
		cv::destroyWindow(OPENCV_WINDOWS_NAME);
	return true;
}

//! 绘制框
void Intermediary::DrawBox(cv::Mat& img) {
	//! 绘制框
	if (parame->showWindows) {
		for (size_t i = 0; i < process.indices.size(); ++i) {
			cv::rectangle(img, cv::Rect(process.boxes[int(process.indices[i])].x - (process.boxes[int(process.indices[i])].width * 0.5f),
				process.boxes[int(process.indices[i])].y - (process.boxes[int(process.indices[i])].height * 0.5f),
				process.boxes[int(process.indices[i])].width, process.boxes[int(process.indices[i])].height),
				cv::Scalar(0, 255, 0), 2, 8, 0);
		}
		cv::imshow(OPENCV_WINDOWS_NAME, img);
		cv::waitKey(1);
	} else {
		if (cv::getWindowProperty(OPENCV_WINDOWS_NAME, cv::WND_PROP_VISIBLE))
			cv::destroyWindow(OPENCV_WINDOWS_NAME);
	}
}

//! 释放
void Intermediary::Release() {
	//! 释放DX
	if (dxgi) {
		dxgi->Release();
		dxgi = nullptr;
	}
	//! 释放frame
	if (frame) {
		frame->Release();
		frame = nullptr;
	}
	//! 释放yolo
	if (yolo) {
		yolo->Release();
		yolo = nullptr;
	}
	//! 释放lock
	if (lock) {
		lock->Release();
		lock = nullptr;
	}
	delete this;
}

//! 获取日志对象
std::shared_ptr<spdlog::logger> Intermediary::getLogger() {
	return g_logger;
}

/*
	记事本
	- 修改执行器为DLL
	- 增加自瞄模块
	- 增加插件入口函数
*/


//! 导出C函数 主线程
EXPORT bool start_thread(bool is_local = true) {

	Intermediary* intermediary  = new Intermediary();

	//! 初始化共享内存
	if (!intermediary->InitSharedParame())  goto Free;
	//! 初始化日志, 先初始化共享内存
	if (is_local) {
		intermediary->InitLogger();
	}
	//! 初始化配置表
	if (!intermediary->InitYoloTable())	goto Free;
	//! 初始化frame
	if (!intermediary->InitFrame())	goto Free;
	//! 初始化模型
	if (!intermediary->InitModel())	goto Free;
	//! 初始化DX截图
	if (!intermediary->InitDX()) goto Free;
	//! 初始化自瞄
	if (!intermediary->InitLock()) goto Free;
	//! 循环推理
	intermediary->Detection();

Free:
	//! 释放
	intermediary->Release();

	return true;
}

//! 入口函数
#if 1 // test
int main() {
	std::cout << " test" << std::endl;
	start_thread(true);

	return 0;
}
#endif