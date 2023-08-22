#pragma once
#include "factory-base.h"
#include "actuator.h"
#include "sf-trt.h"
#include "utils.h"
#include "config-module.h"


class uiBase {
public:

	virtual IStates Init() = 0;

	virtual void Rendering() = 0;

	virtual void GetParame() =0;

	uiBase(Actuator* actuator,Parameter* parame, std::shared_ptr<spdlog::logger> logger) : _actuator(actuator), _parame(parame),_logger(logger) {}

protected:
	Actuator* _actuator = nullptr;
	Parameter* _parame = nullptr;
	std::shared_ptr<spdlog::logger> _logger;

};



// -------------  ui ¹¤³§ ------------- //
class uiFactory : public IFactory {
public:
	static uiFactory* createUiFactory() {
		return new uiFactory();
	}

	IStates QueryInterface(void** ui) override;

	IStates AcquireParamePtr(Parameter* parame);

	IStates AcquireLoggerPtr(std::shared_ptr<spdlog::logger> logger);

	IStates AcquireActuatorPtr(Actuator* _actuator);

	void Release() override {
		delete this;
	}
private:
	uiFactory() {}
	std::shared_ptr<spdlog::logger> _logger;
	Actuator* _actuator;
	Parameter* _parame = nullptr;
};

   

