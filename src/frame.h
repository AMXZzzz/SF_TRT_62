#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include "yolo.h"
#include "IStates.h"

#ifndef API_EXPORT
#define SF_API extern "C" __declspec(dllimport)
#else
#define SF_API extern "C" __declspec(dllexport)
#endif

//! 框架类型
namespace sf {
	namespace Type {
		//! 框架类别
		enum FrameType :int {
			TRT_FRAME = 0,
			DML_FRAME = 1,
		};
	}
}

//! 需要填充该结构体
struct FRAMEINFO{
	YOLO* yolo;								//! yolo配置表
	sf::Type::FrameType frame_type;			//! 框架类型
	std::shared_ptr<spdlog::logger> logger;	//! 日志指针
	int equipment;							//! 运行设备
};

//! 框架基类
class Frame {
public:
	//! 初始化框架需要传入yolo配置表，日志指针，运行设备
	Frame(YOLO* yolo, const std::shared_ptr<spdlog::logger>& logger, int equipment)
		: m_yolo(yolo), m_logger(logger), m_equipment(equipment) {
	}

	//! 解析模型
	virtual bool AnalyticalModel(const char* engine_path) = 0;

	//! 解析模型
	virtual bool AnalyticalModel(const std::string engine_path) = 0;

	//! 推理接口
	virtual void Detect(cv::Mat&) = 0;

	//! 释放接口
	virtual void Release() = 0;

	//! 获取错误信息
	virtual	IStates getLastErrorInfo() = 0;

	//! 虚基类析构
	virtual ~Frame() {};

protected:
	YOLO* m_yolo;										//! yolo配置表
	std::shared_ptr<spdlog::logger> m_logger = nullptr;	//! 指向日志的指针
	int m_equipment = 0;								//! 运行设备
};


namespace sf {
	SF_API Frame* CreateFrame(FRAMEINFO* info);
}