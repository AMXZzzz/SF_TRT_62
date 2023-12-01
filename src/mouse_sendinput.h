#pragma once
#include "mouse_base.h"
#include "Windows.h"

class ISendInput: public IMouse {
public:
	ISendInput(void* data) {}
	//! 初始化移动
	IStates init() override;
	//! 移动方法
	IStates move(int x, int y) override;
	//! 释放
	IStates close() override;

private:
	INPUT input;
};
