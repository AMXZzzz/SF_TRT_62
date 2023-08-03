#pragma once
#include "yolo.h"

class Frame {
public:
	//! 解析模型,必须重写
	virtual bool AnalyticalModel(const char* engine_path) = 0;

	virtual bool AnalyticalModel(const std::string engine_path) = 0;

	//! 推理接口,必须重写
	virtual void Detect(cv::Mat&) = 0;

	//! 释放接口,必须重写
	virtual void Release() = 0;

	//! 获取错误信息,必须重写
	virtual	IStates getLastErrorInfo() = 0;

protected:
	YOLO* _yolo;							//! yolo配置表
	std::shared_ptr<spdlog::logger> _logger = nullptr;	//! 指向日志的指针
	int _equipment = 0;									//! 运行设备

public:
	//! 释放父类指针指向的对象
	//! 若没有虚析构,则只会释放基类本身内存
	//! 出现无法释放派生类的内存
	virtual ~Frame() {};
	Frame(YOLO* yolo)
	: _yolo(yolo){}
	Frame(YOLO* yolo, const std::shared_ptr<spdlog::logger>& logger)
	: _yolo(yolo), _logger(logger) {}	
	Frame(YOLO* yolo, const std::shared_ptr<spdlog::logger>& logger, int equipment)
	: _yolo(yolo), _logger(logger), _equipment(equipment) {
	}
};

class FrameFactory :public IFactory {
public:

	virtual IStates AcquireYoloPtr(YOLO* yolo) {
		_yolo = yolo;
		return IStates();
	};

	virtual IStates AcquireLoggerPtr(std::shared_ptr<spdlog::logger> logger) {
		_logger = logger;
		return IStates();
	};

	virtual IStates AcquireEquipmen(int equipment) {
		_equipment = equipment;
		return IStates();
	};

	virtual ~FrameFactory() {};

protected:
	FrameFactory() {}
	YOLO* _yolo;
	std::shared_ptr<spdlog::logger> _logger;
	int _equipment = 0;
};

