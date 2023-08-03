#pragma once
#include "imgui-module.h"

class Intermediary {
public:
	//! ��ʼ��
	bool init();

	//! ui
	bool startui();

private:
	uiBase* ui = nullptr;
	Actuator* _actuator = nullptr;
	YOLO* _yolo = nullptr;
	Frame* _frame = nullptr;
	Parameter* _parame = nullptr;
	std::shared_ptr<spdlog::logger> _logger;

	void initlog();
};

