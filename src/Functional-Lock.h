#pragma once
#include "lock.h"
#include "algorithm.h"

//! ����ʽ
class FunctionalLock final : public Lock {
public:
	//! ��ʼ�����鷽ʽ
	bool initLock() override;
	//! �����ƶ�
	void triggerMove() override;
	//! �ͷ�
	void Release() override;

	FunctionalLock(LOCKINFO* info) : Lock(info) {};
private:
	//! �ƶ�
	void(*MoveR)(int, int);
	void(*MoveClose)();

	//! �㷨
	Algorithm algorithm_x;
	Algorithm algorithm_y;
};

