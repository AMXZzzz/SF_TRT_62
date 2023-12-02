#pragma once
#include "mouse_base.h"


class IEasyKeyMouse : public IMouse {
public:
	// Í¨¹ý IMouse ¼Ì³Ð
	IStates init() override;
	IStates move(int x, int y) override;
	IStates close() override;
private:
	HANDLE easy_key_mouse_handle;
};

