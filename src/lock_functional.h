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
	//! �����׼�ǵľ���
	void setContainer(std::vector<float>* distance, std::vector<int>* indices, int idx);
	~Functional(){std::cout << "[debug]: Functional ����������" << std::endl;}
};

