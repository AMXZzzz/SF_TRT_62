#pragma once
#include "mouse_base.h"


class IEasyKeyMouse : public IMouse {
public:
	// ͨ�� IMouse �̳�
	IStates init() override;
	bool monitor(int key) override;
	void trigger() override;
	IStates move(int x, int y) override;
	IStates close() override;
private:
	HANDLE easy_key_mouse_handle;


};

