#pragma once
#include "mouse_base.h"
#include "Windows.h"

class ISendInput: public IMouse {
public:
	ISendInput(void* data) {}
	//! ��ʼ���ƶ�
	IStates init() override;
	//! �ƶ�����
	IStates move(int x, int y) override;
	//! �ͷ�
	IStates close() override;

private:
	INPUT input;
};
