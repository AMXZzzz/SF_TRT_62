
#include <atltime.h>
#include "intermediary.h"
#include "trt-module.h"
#include "dml-module.h"


void Intermediary::initlog() {
	// ��ʼ����־
	CTime t = CTime::GetCurrentTime();
	static char temp[MAX_PATH]{};
	_snprintf_s(temp, MAX_PATH, "logs/%d-%d-%d %d-%d-%d.txt", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
	_logger = spdlog::basic_logger_mt("SF_TRT", temp);
	_logger->flush_on(spdlog::level::trace);
}

bool Intermediary::init() {
	// ��ʼ����־
	initlog();

	//! ��ʼ���������ָ��
	ParameFactory* parame_factory = ParameFactory::createParameFactory();
	IStates hr = parame_factory->QueryInterface(reinterpret_cast<void**>(&_parame));
	if (hr.is_error()) {
		std::cout << "������Ϣ:" << hr.msg() << std::endl;
		return false;
	}
	parame_factory->Release();

	//! ��ʼ��ִ����
	actuatorFactory* actuator_factory = actuatorFactory::createActuatorFactory();
	actuator_factory->AcquireLoggerPtr(_logger);
	actuator_factory->AcquireParamePtr(_parame);
	hr = actuator_factory->QueryInterface(reinterpret_cast<void**>(&_actuator));
	if (hr.is_error()) {
		std::cout << "������Ϣ:" << hr.msg() << std::endl;
		return false;
	}
	actuator_factory->Release();

	return true;
}

bool Intermediary::startui() {

	// ��ȡui����
	uiFactory* ui_factory = uiFactory::createUiFactory();
	ui_factory->AcquireParamePtr(_parame);
	ui_factory->AcquireLoggerPtr(_logger);
	ui_factory->AcquireActuatorPtr(_actuator);
	ui_factory->QueryInterface(reinterpret_cast<void**>(&ui));
	ui_factory->Release();

	//! ��ʼ��ui
	ui->Init();
	//! ��ʼ������
	ui->GetParame();

	//! uiѭ��
	while (_parame->uiStop) {
		ui->Rendering();
	}
	return false;
}
