#include "Actuator.h"


#define MAX_SIZE 1024						//! 共享内存大小
#define MapFileName "sf_61_shared_memory"   //! 共享内存ID

//! 初始化共享内存
bool initmem(SharedMemory** signal) {
	HANDLE MapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SIZE, MapFileName);
	if (MapFile == NULL) {
		return false;
	}

	*signal = (SharedMemory*)MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE);
	if (*signal == NULL) {
		return false;
	}
	
	return true;
}

//! 拟放在obs插件中
void test() {
	//! 初始化共享内存
	SharedMemory* sharedmemory;
	if (!initmem(&sharedmemory)) return;  //! 一定要在UI端初始化共享内存，否则退出

	//! 创建执行器
	Actuator ac(sharedmemory);


	//! 共享内存创建时，值是不定值，如signal->ai_start_signal 可能是false,也可能是true
	//! 固定值，尝试：ui端已经初始化共享内存，则跳过初始化
	sharedmemory->s_signal.dll_exit_signal = false;								//! dll 退出信号
	sharedmemory->s_signal.ai_start_signal = true;								//! 执行器开始信号
	sharedmemory->s_signal.show_detect_window = true;							//! 显示检测窗口信号
	sharedmemory->s_info.frame_type = 1;										//! 推理框架
	sharedmemory->s_info.yolo_tyoe =  0;										//! yolo类型
	sharedmemory->s_data.conf =  0.3;											//! 置信度
	sharedmemory->s_data.iou =  0.1;											//! iou置信度
	sharedmemory->s_info.model_path = "cf_yolov5s_15w_640_2label.onnx";			//! 模型路径

	//! 监听循环
	while (sharedmemory->s_signal.dll_exit_signal == false) {

		//! 启动AI线程
		if (sharedmemory->s_signal.ai_start_signal == true) {
			ac.start();	//! 启动AI
			sharedmemory->s_signal.ai_start_signal = false;	//! 复位信号，不然一直在创建线程
		}

		//! 监听到退出dll信号
		if (sharedmemory->s_signal.dll_exit_signal == true) {
			sharedmemory->s_signal.dll_exit_signal = false;		//! 复位信号,不然执行到这就自动退出
			ac.exit();		//! 使执行器线程退出
			ac.join();		//! 等待执行器退出
			//! 卸载自身dll，如果有
			
			//! 释放共享内存,
			
		}
	}
}

//! 测试
int main() {
	test();
	return 0;
}