#pragma once
#include "lock_base.h"
#include "IState.h"
#include "yolo_base.h"


class Functional: public LOCK {
public:
	Functional(LockInfo info):LOCK(info){
		std::cout << "[debug]: Functional 派生类构造" << std::endl;
	}
	//! 初始化lock 
	IStates initLock() override;
	//! 开始动作
	void action() override;
	//! 释放
	void Release() override;
private:
	//! 计算和准星的距离
	void setContainer(std::vector<float>* distance, std::vector<int>* indices, int idx);
	~Functional(){std::cout << "[debug]: Functional 派生类析构" << std::endl;}
};

