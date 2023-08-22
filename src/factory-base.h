#pragma once
#include "sf-trt.h"

//! ------------------ ������  ���й����Ļ���  ------------------ //
class IFactory {
public:
	//! ��������ӿ�
	virtual IStates QueryInterface(void**) = 0;
	//! �ͷ�
	virtual void Release() = 0;

	virtual ~IFactory() {};

protected:

};




