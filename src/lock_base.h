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
#include "mouse_base.h"
#include "sf-trt.h"
#include "base_type.h"

namespace sf {
	namespace Type {
		enum LockManner :int {
			Functional = 0,
			Multithread = 1,
		};
	}
}

struct TargetInfo {		//! 最近目标信息
	float origin_x;		//! 目标box原点x
	float origin_y;		//! 目标box原点y
	float width;		//! 目标box宽
	float height;		//! 目标box高
	float center_x;		//! 目标box中心点x
	float center_y;		//! 目标box中心点x
	float distance_x;	//! 目标相对距离x,原始距离
	float distance_y;	//! 目标相对距离y，原始距离
	float move_x;		//! 目标x需要移动的距离，计算距离
	float move_y;		//! 目标x需要移动的距离，计算距离
};

struct LockInfo {
	sf::Type::LockManner manner;	//! 自瞄的方式
	MouseInfo mouse_info;			//! 鼠标执行方式
	Process* process;				//! 处理结构体
	SharedMemory* sharedmemory;		//! 共享内存指针
	IPoint* point;
};

class  LOCK {
public:
	LOCK(LockInfo info): m_point(info.point),m_mouse_info(info.mouse_info), m_process(info.process), m_sharedmemory(info.sharedmemory){
		std::cout << "[debug]: LOCK基类构造" << std::endl;
		if (m_process == nullptr) {std::cout << "[debug]: 传入LOCK的process指针为空" << std::endl;}
		if (m_sharedmemory == nullptr) {std::cout << "[debug]: 传入LOCK的sharedmemory指针为空" << std::endl;}
	}
	//! 初始化lock 
	virtual IStates initLock() = 0;
	//! 执行自瞄
	virtual void action() = 0;
	//! 释放
	virtual void Release() = 0;
	virtual ~LOCK() { 
		m_mouse->close();
		std::cout << "[debug]: LOCK基类释放" << std::endl;
	};
protected:
	IPoint* m_point;
	IMouse* m_mouse;				//! 鼠标对象
	Process* m_process;				//! 先验框
	MouseInfo m_mouse_info{};		//! 鼠标配置
	SharedMemory* m_sharedmemory;	//! 共享内存，优化单信号
private:
	LOCK() {};	//! 禁用默认构造
};

namespace sf {
	LOCK* createLock(LockInfo info);
}

