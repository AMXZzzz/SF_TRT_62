#pragma once
#include "lock_base.h"
#include "IState.h"


class Functional: public LOCK {
public:
	Functional(MouseInfo mouseinfo):LOCK(mouseinfo){
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

