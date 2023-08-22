#pragma once
#include "sf-trt.h"

//! ------------------ 纯虚类  所有工厂的基类  ------------------ //
class IFactory {
public:
	//! 创建对象接口
	virtual IStates QueryInterface(void**) = 0;
	//! 释放
	virtual void Release() = 0;

	virtual ~IFactory() {};

protected:

};




