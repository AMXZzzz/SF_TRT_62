
#include <atltime.h>
#include "intermediary.h"
#include "trt-module.h"
#include "dml-module.h"


void Intermediary::initlog() {
	// 初始化日志
	CTime t = CTime::GetCurrentTime();
	static char temp[MAX_PATH]{};
	_snprintf_s(temp, MAX_PATH, "logs/%d-%d-%d %d-%d-%d.txt", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
	_logger = spdlog::basic_logger_mt("SF_TRT", temp);
	_logger->flush_on(spdlog::level::trace);
}

bool Intermediary::init() {
	// 初始化日志
	initlog();

	//! 初始化共享参数指针
	ParameFactory* parame_factory = ParameFactory::createParameFactory();
	IStates hr = parame_factory->QueryInterface(reinterpret_cast<void**>(&_parame));
	if (hr.is_error()) {
		std::cout << "错误信息:" << hr.msg() << std::endl;
		return false;
	}
	parame_factory->Release();

	//! 初始化执行器
	actuatorFactory* actuator_factory = actuatorFactory::createActuatorFactory();
	actuator_factory->AcquireLoggerPtr(_logger);
	actuator_factory->AcquireParamePtr(_parame);
	hr = actuator_factory->QueryInterface(reinterpret_cast<void**>(&_actuator));
	if (hr.is_error()) {
		std::cout << "错误信息:" << hr.msg() << std::endl;
		return false;
	}
	actuator_factory->Release();

	return true;
}

bool Intermediary::startui() {

	// 获取ui对象
	uiFactory* ui_factory = uiFactory::createUiFactory();
	ui_factory->AcquireParamePtr(_parame);
	ui_factory->AcquireLoggerPtr(_logger);
	ui_factory->AcquireActuatorPtr(_actuator);
	ui_factory->QueryInterface(reinterpret_cast<void**>(&ui));
	ui_factory->Release();

	//! 初始化ui
	ui->Init();

	//! ui循环
	while (_parame->uiStop) {
		ui->Rendering();
	}

	return false;
}
