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

// 宏重载
#define TRTHEAD "[DLL]: "
#define LOGINFO(format,...) if (m_logger) {m_logger->info(TRTHEAD ## format, __VA_ARGS__);} else {std::cout<<"spdlog未初始化"<<std::endl;}
#define LOGWARN(format,...)  if (m_logger) {m_logger->warn(TRTHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlog未初始化"<<std::endl;}
#define LOGERROR(format,...) if (m_logger) {m_logger->error(TRTHEAD ## format, __VA_ARGS__);}else {std::cout<<"spdlog未初始化"<<std::endl;}
#define CHECK_TRT(x) (x==NULL)

sf::Type::YoloType convertYoloType(int type) {
	switch (type) {
	case 0: return sf::Type::YoloType::TYPE_YOLOV5;
	case 1: return sf::Type::YoloType::TYPE_YOLOV8;
	case 2: return sf::Type::YoloType::TYPE_YOLOX;
	}
	//! 默认使用yolov5
	return sf::Type::YoloType::TYPE_YOLOV5;
}

sf::Type::FrameType convertFrameType(int type) {
	switch (type) {
	case 0: return sf::Type::FrameType::TRT_FRAME;
	case 1: return sf::Type::FrameType::DML_FRAME;
	}
	//! 默认使用DML
	return sf::Type::FrameType::DML_FRAME;
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

bool Actuator::setYoloType(int type) {
	//! 设置yolo属性
	YOLOINFO yolo_info{};
	yolo_info.conf = &m_sharedmemory->s_data.conf;
	yolo_info.iou = &m_sharedmemory->s_data.iou;
	yolo_info.type = convertYoloType(type);
	yolo_info.process = m_process;

	//! 创建yolo
	m_yolo = sf::createYoloTable(&yolo_info);
	return true;
}

bool Actuator::setFrameBack(int type,int equipment) {
	//! 设置框架属性
	FRAMEINFO frame_info{};
	frame_info.equipment = equipment;
	frame_info.yolo = m_yolo;
	frame_info.logger = m_logger;
	frame_info.frame_type = convertFrameType(type);
	//! 创建推理框架
	m_frame = sf::createFrame(&frame_info);
	return false;
}

bool Actuator::InitializeResources() {
	//! 日志是否初始化过了,表明没有打开UI端
	if (m_logger) {
		if (!setSpdlog()) { 
			return false;
		}
	}
	//! yolo类型

	//! 初始化框架

	return false;
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
	//! 初始化ALL
	setSpdlog();

	//! 运行
	while (m_exit_signal == false) {
		//! 截图
		std::cout << "run" << std::endl;
		//! 推理

		//! 后处理
	}

	//! 释放资源
}
