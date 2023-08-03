#pragma once
#include <windows.h>
#include "factory-base.h"

struct Parameter {
	float conf = 0.3;
	float iou = 0.1;
	Process process;
	bool uiStop = true;
	bool executionStatus = true;

	bool ai_is_run = false;

	//! �ǹ����ڴ�
	bool is_shared() {
		return _shared == true;
	}

	void Release() {
		if (is_shared()) UnmapViewOfFile(this);
		CloseHandle(this->_MapFile);
		delete this;
	}

	void setMapFile(HANDLE MapFile) {
		_MapFile = MapFile;
	}

private:
	bool _shared = false;
	HANDLE _MapFile = NULL;		// ��¼Mapָ��,�������ͷ�

};



class ParameFactory final : public IFactory {
public:
	static ParameFactory* createParameFactory() {
		return new ParameFactory();
	}

	//! ��ȡ����
	IStates QueryInterface(void** parme) override;

	//! ����Ϊ�����ڴ�
	void setSharedMemory();

	//! �ͷ�
	void Release() override;

private:
	HANDLE _MapFile;
	bool is_sharedmemory = false;

	ParameFactory() {}
};
