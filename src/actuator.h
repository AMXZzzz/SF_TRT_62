#pragma once
#include "factory-base.h"
#include "frame-base.h"
#include "parameter.h"

class Actuator {
public:

	//! ��ʼִ��
	bool executionThread(int yolotype, int backend, char*);

	//! ��ȡ����ָ��
	Parameter* getParameterPtr();

	//! ��ȡ��־
	std::shared_ptr<spdlog::logger> getLoggerPtr();

	//! ��ȡ���
	YOLO* getYoloConfig();
	//! ��ȡ���ñ�
	Frame* getFrame();

	void Release() {
		delete this;
	}

	Actuator(Parameter* parame, std::shared_ptr<spdlog::logger> logger) : _parame(parame), _logger(logger) {}

private:
	HANDLE thread;
	Parameter* _parame;
	std::shared_ptr<spdlog::logger> _logger;
	YOLO* _yolo = nullptr;
	Frame* _frame = nullptr;
};


class actuatorFactory final :public IFactory {
public:
	static actuatorFactory* createActuatorFactory() {
		return	new actuatorFactory();
	}

	IStates QueryInterface(void** actuator);

	IStates AcquireParamePtr(Parameter* parame);

	IStates AcquireLoggerPtr(std::shared_ptr<spdlog::logger> logger);

	void Release() override {
		delete this;
	}

private:
	std::shared_ptr<spdlog::logger> _logger;
	Parameter* _parame = nullptr;
};

