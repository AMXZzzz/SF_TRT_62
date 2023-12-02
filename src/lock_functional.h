#pragma once
#include "lock_base.h"
#include "IState.h"
#include "yolo_base.h"


class Functional: public LOCK {
public:
	Functional(LockInfo info):LOCK(info){
		std::cout << "[debug]: Functional �����๹��" << std::endl;
	}
	//! ��ʼ��lock 
	IStates initLock() override;
	//! ��ʼ����
	void action() override;
	//! �ͷ�
	void Release() override;
private:
	~Functional(){std::cout << "[debug]: Functional ����������" << std::endl;}
};

