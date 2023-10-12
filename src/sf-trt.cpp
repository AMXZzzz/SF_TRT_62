#include "Actuator.h"


#define MAX_SIZE 1024
#define MapFileName "sf_61_shared_memory"

bool initmem(MemorySignal** signal) {
	HANDLE MapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SIZE, MapFileName);
	if (MapFile == NULL) {
		return false;
	}

	*signal = (MemorySignal*)MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE);
	if (*signal == NULL) {
		return false;
	}

	//! 共享内存创建时，值是不定值，如signal->ai_start_signal 可能是false,也可能是true
	//! 这里使用字符串来判断是否初始化过，如果ui端未初始化，则在dll端初始化
	//! 如果ui端已经初始化共享内存，则跳过初始化
	if ((*signal)->shared_memory_succecc) {
		(*signal)->conf = 0.3;
		(*signal)->iou = 0.1;
		(*signal)->ai_start_signal = false;
		(*signal)->dll_exit_signal = false;
		(*signal)->shared_memory_succecc = true;
	}
	return true;
}

//! 拟放在obs插件中
void test() {
	//! 初始化共享内存
	MemorySignal* signal;
	if (!initmem(&signal)) return;

	//! 创建执行器
	Actuator ac(signal);

	//! 监听循环
	while (signal->dll_exit_signal == false) {

		//! 启动AI线程
		if (signal->ai_start_signal == true) {
			ac.start();	//! 启动AI
			signal->ai_start_signal = false;	//! 复位信号，不然一直在创建线程
		}

		//! 监听到退出dll信号
		if (signal->dll_exit_signal == true) {
			signal->dll_exit_signal = false; //! 复位信号,不然加载dll执行到这就自动退出
			ac.exit();		//! 使执行器线程退出
			ac.join();		//! 等待执行器退出
			//! 卸载自身dll，如果有
			
			//! 释放共享内存
		}
	}
}

//! 测试
int main() {
	test();

	return 0;
}