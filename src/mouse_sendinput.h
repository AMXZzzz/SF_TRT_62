#pragma once
#include "mouse_base.h"
#include "Windows.h"

class ISendInput: public IMouse {
public:
	//! 初始化移动
	IStates init() override;
	//! 移动方法
	IStates move(int x, int y) override;
	//! 释放
	IStates close() override;

private:
	INPUT input;

	// 通过 IMouse 继承
	bool monitor(int key) override;
	void trigger() override;
};
