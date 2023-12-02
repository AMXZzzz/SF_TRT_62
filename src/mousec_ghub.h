#pragma once

#include "mouse_base.h"
#include <winnt.h>

class IGHUB: public IMouse {
public:

	// ͨ�� IMouse �̳�
	IStates init() override;
	IStates move(int x, int y) override;
	IStates close() override;
private:
	HANDLE ghub_handle;
};
