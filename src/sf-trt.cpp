/*
* 免责声明：
* 本软件是开源程序，您可以自由使用、修改和发布它，使用 GNU 通用公共许可证版本 2（GPLv2）的条款。
* 请注意，在根据GPLv2发布本软件之前，您必须遵守以下条款：
* 1. 任何修改和派生作品必须同样采用GPLv2，并在您的派生作品中提供GPLv2的完整文本。
* 2. 您必须保留原始代码的版权通告，并在您的项目中明确指明原始作者和软件的来源。
* 3. 如果您分发本软件，您必须提供完整的源代码，以满足GPLv2的要求。这包括您的修改和派生作品的源代码。
* 4. 本软件是按"原样"提供的，不附带任何明示或暗示的保证。作者对于使用本软件造成的任何直接、间接、偶发、特殊、典型或惩戒性损害不承担责任。
* 使用此代码即表示您同意遵守GPLv2的所有要求和免责声明的条件。
* 请仔细阅读GNU通用公共许可证版本2以了解更多详细信息。您可以在 http ://www.gnu.org/licenses/gpl-2.0.html 上找到GPLv2的完整文本。
* [作者姓名]: bilibili：随风而息
* [地址]:https://github.com/AMXZzzz/SF_TRT_62.git
* [日期]: 2023/10/26
*/
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
	sharedmemory->s_info.frame_type = 0;										//! 推理框架
	sharedmemory->s_info.yolo_tyoe =  0;										//! yolo类型
	sharedmemory->s_data.conf =  0.3;											//! 置信度
	sharedmemory->s_data.iou =  0.1;											//! iou置信度
	sharedmemory->s_info.model_path = "ckbq_yolov5n_2w5.engine";				//! 模型路径

	//! 监听循环
	while (sharedmemory->s_signal.dll_exit_signal == false) {

		//! 启动AI线程信号
		if (sharedmemory->s_signal.ai_start_signal == true) {
			ac.start();	//! 启动AI
			sharedmemory->s_signal.ai_start_signal = false;	//! 复位信号，不然一直在创建线程
		}

		//! 监听到退出dll信号
		if (sharedmemory->s_signal.dll_exit_signal == true) {
			sharedmemory->s_signal.dll_exit_signal = false;		//! 复位信号,不然执行到这就自动退出
			ac.exit();		//! 使执行器线程退出
			ac.join();		//! 等待执行器退出
			//! 释放
			ac.Release();
			//! 卸载自身dll，如果有
		}
	}
}

//! 测试
int main() {
	test();
	return 0;
}