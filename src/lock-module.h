#pragma once
#include "control-algorithm.h"
#include "parameter.h"

struct LockMode {
	static LockMode& Get() {
		static LockMode m_pInstance;
		return m_pInstance;
	}
	BOOL InitLock(int);
	VOID StratLock(Parameter* parame);
	VOID Release();	
};
