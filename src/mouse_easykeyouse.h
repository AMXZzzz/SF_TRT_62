#pragma once
#include "mouse_base.h"


class IEasyKeyMouse : public IMouse {
public:
	// ͨ�� IMouse �̳�
	IStates init() override;
	IStates move(int x, int y) override;
	IStates close() override;
private:
	HANDLE easy_key_mouse_handle;
};

