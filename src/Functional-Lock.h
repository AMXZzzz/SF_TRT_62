#pragma once
#include "lock.h"
#include "algorithm.h"

//! 函数式
class FunctionalLock final : public Lock {
public:
	//! 初始化自瞄方式
	bool initLock() override;
	//! 触发移动
	void triggerMove() override;
	//! 释放
	void Release() override;

	FunctionalLock(LOCKINFO* info) : Lock(info) {};
private:
	//! 移动
	void(*MoveR)(int, int);
	void(*MoveClose)();

	//! 算法
	Algorithm algorithm_x;
	Algorithm algorithm_y;
};

