#pragma once
#include "yolo.h"

class Frame {
public:
	//! ����ģ��,������д
	virtual bool AnalyticalModel(const char* engine_path) = 0;

	virtual bool AnalyticalModel(const std::string engine_path) = 0;

	//! ����ӿ�,������д
	virtual void Detect(cv::Mat&) = 0;

	//! �ͷŽӿ�,������д
	virtual void Release() = 0;

	//! ��ȡ������Ϣ,������д
	virtual	IStates getLastErrorInfo() = 0;


protected:
	YOLO* _yolo;							//! yolo���ñ�
	std::shared_ptr<spdlog::logger> _logger = nullptr;	//! ָ����־��ָ��
	int _equipment = 0;									//! �����豸

public:
	//! �ͷŸ���ָ��ָ��Ķ���
	//! ��û��������,��ֻ���ͷŻ��౾���ڴ�
	//! �����޷��ͷ���������ڴ�
	virtual ~Frame() {};

	VOID(*LockStart)();

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

