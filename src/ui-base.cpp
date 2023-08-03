
#include "ui-base.h"
#include "imgui-module.h"

IStates uiFactory::QueryInterface(void** ui) {
	*ui = new imguiUi(_actuator, _parame, _logger);
	return IStates();
}

IStates uiFactory::AcquireActuatorPtr(Actuator* actuator) {
	_actuator = actuator;
	return IStates();
}

IStates uiFactory::AcquireLoggerPtr(std::shared_ptr<spdlog::logger> logger) {
	_logger = logger;
	return IStates();
}


IStates uiFactory::AcquireParamePtr(Parameter* parame) {
	_parame = parame;
	return IStates();
}