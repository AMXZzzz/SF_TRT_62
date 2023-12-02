#pragma once

#include "mouse_base.h"
#include <winnt.h>

class IGHUB: public IMouse {
public:

	// Í¨¹ý IMouse ¼Ì³Ð
	bool monitor(int key) override;
	void trigger() override;
	IStates init() override;
	IStates move(int x, int y) override;
	IStates close() override;
private:
	HANDLE ghub_handle;


};
