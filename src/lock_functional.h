#pragma once
#include "lock_base.h"
#include "IState.h"


class Functional: public LOCK {
public:
	Functional(MouseInfo mouseinfo):LOCK(mouseinfo){
		std::cout << "[debug]: Functional �����๹��" << std::endl;
	}
	//! ��ʼ��lock 
	bool initLock() override;
	//! ��ȡ������Ϣ
	IStates getLastError() override;
	//! ��ʼ����
	void action() override;
	//! �ͷ�
	void Release() override;
private:
	std::vector<IStates> m_states; 
	~Functional(){std::cout << "[debug]: Functional ����������" << std::endl;}
};

