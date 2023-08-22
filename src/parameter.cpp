#include "parameter.h"
#define MAX_SIZE 256
#define MapFileName L"SF_TRT_62"



IStates ParameFactory::QueryInterface(void** parme) {
	if (is_sharedmemory) {
		// 共享内存
		_MapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SIZE, MapFileName);

		if (_MapFile == NULL) {
			return IStates(State::UNKONEERR, "创建映射失败");
		}

		*parme = (Parameter*)MapViewOfFile(_MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE);
		if (*parme == NULL) {
			*parme = nullptr;
			return IStates(State::UNKONEERR, "映射文件失败");
		}
		// 共享内存需要初始化参数  
		((Parameter*)*parme)->setMapFile(_MapFile);
		((Parameter*)*parme)->uiStop = true;
	} else {
		*parme = new Parameter();
	}
	return IStates();
}

void ParameFactory::setSharedMemory() {
	is_sharedmemory = true;
}

void ParameFactory::Release() {
	delete this;
}
