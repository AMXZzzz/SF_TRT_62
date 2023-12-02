/*
* 免责声明：
* 本软件是开源程序，您可以自由使用、修改和发布它，使用 GNU 通用公共许可证版本 2（GPLv2）的条款。
* 请注意，在根据GPLv2发布本软件之前，您必须遵守以下条款：
* 1. 任何修改和派生作品必须同样采用GPLv2，并在您的派生作品中提供GPLv2的完整文本。
* 2. 您必须保留原始代码的版权通告，并在您的项目中明确指明原始作者和软件的来源。
* 3. 如果您分发本软件，您必须提供完整的源代码，以满足GPLv2的要求。这包括您的修改和派生作品的源代码。
* 4. 本软件是按"原样"提供的，不附带任何明示或暗示的保证。作者对于使用本软件造成的任何直接、间接、偶发、特殊、典型或惩戒性损害不承担责任。
* 使用此代码即表示您同意遵守GPLv2的所有要求和免责声明的条件。
* 请仔细阅读GNU通用公共许可证版本2以了解更多详细信息。您可以在 http ://www.gnu.org/licenses/gpl-2.0.html 上找到GPLv2的完整文本。
* [作者姓名]: bilibili：随风而息
* [地址]:https://github.com/AMXZzzz/SF_TRT_62.git
* [日期]: 2023/10/26
*/
#include <atltime.h>
#include "Actuator.h"

#define SHARED_MEMORY_SIZE 1024
#define SHARED_MEMORY_NAME "sf_62_memory_name_v3"

#define asserthr(x) if(!x) return false;

// 宏重载
#define DLLHEAD "[DLL]: "
#define LOGINFO(format,...) if (m_logger) {m_logger->info(DLLHEAD ## format, __VA_ARGS__);} else {std::cout<<"spdlog未初始化"<<std::endl;}
#define LOGWARN(format,...)  if (m_logger) {m_logger->warn(DLLHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlog未初始化"<<std::endl;}
#define LOGERROR(format,...) if (m_logger) {m_logger->error(DLLHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlog未初始化"<<std::endl;}
#define CHECK_TRT(x) (x==NULL)


static sf::Type::YoloType convertYoloType(int type) {
	switch (type) {
	case 0:
		std::cout << "[debug]: YOLOV5" << std::endl;
		return sf::Type::YoloType::TYPE_YOLOV5;
	case 1:
		std::cout << "[debug]: YOLOV8" << std::endl;
		return sf::Type::YoloType::TYPE_YOLOV8;
	case 2:
		std::cout << "[debug]: YOLOX" << std::endl;
		return sf::Type::YoloType::TYPE_YOLOX;
	}
	//! 如果未命中则使用yolov5
	std::cout << "[debug]: YOLOV5" << std::endl;
	return sf::Type::YoloType::TYPE_YOLOV5;
}

static sf::Type::FrameType convertFrameType(int type) {
	switch (type) {
	case 0:
		std::cout << "[debug]: TensorRt" << std::endl;
		return sf::Type::FrameType::TRT_FRAME;
	case 1:
		std::cout << "[debug]: DML" << std::endl;
		return sf::Type::FrameType::DML_FRAME;
	}
	//! 如果未命中则使用DML
	std::cout << "[debug]: DML" << std::endl;
	return sf::Type::FrameType::DML_FRAME;
}

static sf::Type::MousecType convertMousecType(int type) {
	switch (type) {
	case 0:
		std::cout << "[debug]: GHUB" << std::endl;
		return sf::Type::MousecType::GHUB;
	case 1:
		std::cout << "[debug]: EasyKeyMouse" << std::endl;
		return sf::Type::MousecType::EasyKeyMouse;
	case 2:
		std::cout << "[debug]: SendInput" << std::endl;
		return sf::Type::MousecType::SendInput;
	}
	//! 如果未命中则使用SendInput
	std::cout << "[debug]: SendInput" << std::endl;
	return sf::Type::MousecType::SendInput;
}

static sf::Type::LockManner convertLockType(int type) {
	switch (type) {
	case 0:
		std::cout << "[debug]: Functional" << std::endl;
		return sf::Type::LockManner::Functional;
	case 1:
		std::cout << "[debug]: Multithread" << std::endl;
		return sf::Type::LockManner::Multithread;
	}
	//! 如果未命中则使用SendInput
	std::cout << "[debug]: Functional" << std::endl;
	return sf::Type::LockManner::Functional;
}

#define WINDOWS_NAME "test"
void DrawBox(Process* process, cv::Mat& img, bool show) {
	// 画框
	if (!show) {
		if (cv::getWindowProperty(WINDOWS_NAME, cv::WND_PROP_VISIBLE))
			cv::destroyWindow(WINDOWS_NAME);
	}
	else {
		for (int i = 0; i < process->indices.size(); ++i) {
			cv::rectangle(img,
				cv::Rect(
#if CENTER_COORDINATE
					process->boxes[process->indices[i]].x - (process->boxes[process->indices[i]].width * 0.5f),
					process->boxes[process->indices[i]].y - (process->boxes[process->indices[i]].height * 0.5f),
#else
					process->boxes[process->indices[i]].x,
					process->boxes[process->indices[i]].y,
#endif
					process->boxes[process->indices[i]].width,
					process->boxes[process->indices[i]].height),
				cv::Scalar(0, 255, 0), 2, 8, 0);
		}
		cv::imshow(WINDOWS_NAME, img);
		cv::pollKey();
	}
}

bool Actuator::setSpdlog() {
	// 初始化日志
	CTime t = CTime::GetCurrentTime();
	static char temp[MAX_PATH]{};
	_snprintf_s(temp, MAX_PATH, "logs/%d-%d-%d %d-%d-%d.txt", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
	m_logger = spdlog::basic_logger_mt("SF_TRT", temp);
	m_logger->flush_on(spdlog::level::trace);
	return true;
}

bool Actuator::setYoloConfigObject() {
	//! 设置yolo属性
	YOLOINFO yolo_info{};
	yolo_info.conf = &m_sharedmemory->s_data.conf;
	yolo_info.iou = &m_sharedmemory->s_data.iou;
	yolo_info.process = &m_process;
	//! yolo类型，0：yolov5/v7 1：yolov8 2：yolox
	yolo_info.type = convertYoloType(m_sharedmemory->s_info.yolo_tyoe);

	//! 创建yolo
	m_yolo = sf::createYoloTable(&yolo_info);
	if (m_yolo == nullptr) {
		std::cout << "[debug]: setYoloConfigObject失败" << std::endl;
		return false;
	}
	return true;
}

bool Actuator::setDetectFrameworkObject() {
	//! 设置框架属性
	FRAMEINFO frame_info{};
	frame_info.equipment = m_sharedmemory->s_info.equipment;
	frame_info.yolo = m_yolo;
	frame_info.logger = m_logger;
	frame_info.frame_type = convertFrameType(m_sharedmemory->s_info.frame_type);
	//! 创建推理框架
	m_frame = sf::createFrame(&frame_info);
	if (m_frame == nullptr) {
		std::cout << "[debug]: setDetectFrameworkObject失败" << std::endl;
		return false;
	}
	//! 初始化模型
	if (!m_frame->AnalyticalModel(m_sharedmemory->s_info.model_path)) {
		std::cout << "[debug]: 解析模型失败，错误信息: " << m_frame->getLastErrorInfo().getErrorMessage() << std::endl;
		LOGERROR("解析模型失败,错误:{}", m_frame->getLastErrorInfo().getErrorMessage())
		return false;
	}
	return true;
}

bool Actuator::setLockLogicObject() {
	//! 配置鼠标移动
	MouseInfo mouse_info{};
	mouse_info.mousec_manner = convertMousecType(m_sharedmemory->s_info.mousec_type);
	mouse_info.data = "com3";

	//! 配置自瞄信息
	LockInfo lock_info{};
	lock_info.manner = convertLockType(m_sharedmemory->s_info.lock_type);
	lock_info.mouse_info = mouse_info;
	//! 初始化lock对象
	m_lock = sf::createLock(&lock_info);
	if (m_lock == nullptr) {
		std::cout << "[debug]: setLockLogicObject失败" << std::endl;
		return false;
	}
	IStates hr = m_lock->initLock();
	//! 初始化lock
	if (hr.is_error()) {
		std::cout << "[debug]:" << m_lock->getLastError().getErrorMessage() << std::endl;
		LOGERROR("初始化Lock失败:{}",m_lock->getLastError().getErrorMessage())
		return false;
	}
	return true;
}

bool Actuator::setDXGICpatureObject() {
	m_dx = sf::createDxgi(&m_point);
	SF_DXGI_ERROR hr = m_dx->SetCaptureResource(m_yolo->getImageSize().width, m_yolo->getImageSize().height);
	if (DXGI_SUCCECC != hr) {
		switch (hr) {
		case DXGI_DEVICE_ERROR:
			std::cout << "[debug]: setDxgiCpature失败，错误：初始化d3ddevice失败" << std::endl;
			LOGERROR("初始化d3ddevice失败")
			break;
		case DXGI_DEVICE2_ERROR:
			std::cout << "[debug]: setDxgiCpature失败，错误：初始化d3ddevice2失败" << std::endl;
			LOGERROR("初始化d3ddevice2失败")
			break;
		case DXGI_DUPLICATE_ERROR:
			std::cout << "[debug]: setDxgiCpature失败，错误：初始化d3d显示器信息失败" << std::endl;
			LOGERROR("初始化d3d输出信息失败, 请将程序设置在直连显示器的GPU上")
			break;
		case DXGI_FACTORY_ERROR:
			std::cout << "[debug]: setDxgiCpature失败，错误：创建工厂模板失败" << std::endl;
			LOGERROR("创建工厂模板失败")
			break;
		}
		return false;
	}

	return true;
}

bool Actuator::initializeResources() {
	//! 日志是否初始化过
	setSpdlog();
	//! yolo类型对象
	asserthr(setYoloConfigObject());
	//! 初始化框架对象
	asserthr(setDetectFrameworkObject());
	//! 初始化截图对象
	asserthr(setDXGICpatureObject());
	//! 初始化自瞄对象
	asserthr(setLockLogicObject());
	return true;
}

void Actuator::start() {
	if (!actuatorThreadHandle.joinable()) {
		actuatorThreadHandle = std::thread(&Actuator::word, this);
	}
	else {
		std::cout << "Actuator::word线程已启动" << std::endl;
	}
}

void Actuator::exit() {
	m_exit_signal = true;
}

void Actuator::join() {
	if (!actuatorThreadHandle.joinable()) {
		actuatorThreadHandle.join();
		std::cout << "Actuator 线程已退出" << std::endl;
	}
	else {
		std::cout << "Actuator线程未启动" << std::endl;
	}
}

void Actuator::word() {
	//! 初始化
	if (initializeResources()) {
		//! 运行
		cv::Mat img;
		while (m_exit_signal == false) {
			//! 截图
			if (m_dx->GetBitmapToMat(&img) != DXGI_SUCCECC) {
				continue;
			}
			//! 推理
			m_frame->Detect(img);
			//! 自瞄
			m_lock->action();
			//! 后处理
			DrawBox(&m_process, img, m_sharedmemory->s_signal.show_detect_window);
		}
	}

	//! 释放资源
	Release();
}

void Actuator::Release() {
	m_dx->Release();
	m_yolo->Release();
	m_frame->Release();
	m_lock->Release();
}