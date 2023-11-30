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

#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include <thread>
#include "yolo_base.h"
#include "frame_base.h"
#include "sf-trt.h"
#include "dxgi-module.h"
#include "lock_base.h"

class Actuator {
public:
	//! 启动线程
	void start();
	//! 退出线程
	void exit();
	//! 等待线程退出
	void join();
	//! 有参构造
	Actuator(SharedMemory* sharedmemory) : m_sharedmemory(sharedmemory){
		//! 
		if (m_sharedmemory == nullptr)  std::cout << "传入 Actuator 的 SharedMemory 指针为空" << std::endl;
	}
	//! 释放
	void Release();
private:
	DXGI* dx;									//! dx截图对象
	YOLO* m_yolo;								//! yolo类型基类
	Frame* m_frame;								//! 推理后端框架
	SharedMemory* m_sharedmemory;				//! 共享内存信号
	std::shared_ptr<spdlog::logger> m_logger;	//！日志智能指针
	LOCK* lock;									//! 自瞄逻辑对象

	Process m_process;							//! 预（后）处理容器
	IPoint m_point;								//! 坐标点信息
	bool m_exit_signal = false;					//! 退出中间变量
	std::thread actuatorThreadHandle;			//! 线程句柄

	//! 初始化日志，尝试：优化至UI端
	bool setSpdlog();
	//! 初始化yolo框架类型
	bool setYoloConfigObject();
	//! 初始化推理后端
	bool setDetectFrameworkObject();
	//! 初始化dxgi
	bool setDXGICpatureObject();
	//! 设置lock对象
	bool setLockLogicObject();
	//! 初始化资源
	bool initializeResources();
	//! 工作线程
	void word();
	//! 禁用默认构造
	Actuator() {}
};

