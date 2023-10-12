#include <windows.h>
#include "multithread-Lock.h"

void chooseClass(std::vector<float>& EuclideanDistance,
	std::vector<int>& indices,
	Process* process,
	IPoint* point,
	const int& idx,
	const bool& classes) {
	if (classes) {
		indices.push_back(process->indices[idx]);
		EuclideanDistance.push_back(
			pow((point->origin_x + process->boxes[process->indices[idx]].x) - point->center_x, 2) +
			pow((point->origin_y + process->boxes[process->indices[idx]].y) - point->center_y, 2)
		);
	}
}

typedef std::chrono::system_clock::time_point ITIME_POINT;
static ITIME_POINT auto_start = std::chrono::system_clock::now();
static ITIME_POINT sleep_start = std::chrono::system_clock::now();
inline bool AutoComplete(IParameter* parame, ITIME_POINT start) {
	int random_time = 0;
	if (parame->auto_random != 0) {
		random_time = (int)parame->auto_random * rand() / (RAND_MAX + 1);
	}
	ITIME_POINT auto_end = std::chrono::system_clock::now();
	return std::chrono::duration_cast <std::chrono::milliseconds> (auto_end - start).count() >= (parame->auto_interval + random_time) * 1.0;
}

static inline bool IGetAsyncKeyState(IParameter* parame) {
	//! 监听方式
	switch (parame->monitor_module) {
	case 0:			//! API
		if (parame->lock_key2 == 0) {
			return GetAsyncKeyState(parame->lock_key2) && parame->lock_switch;
		} else {
			return (GetAsyncKeyState(parame->lock_key) && parame->lock_switch) || GetAsyncKeyState(parame->lock_key2);

		}
	case 1:		//! 硬件监听
		if (parame->lock_key2 == 0) {
			return  parame->conitor(parame->lock_key) && parame->lock_switch;
		} else {
			return (parame->conitor(parame->lock_key) && parame->lock_switch) || parame->conitor(parame->lock_key2);
		}
	}
}

static inline void LeftButtonClick() {
	INPUT input{};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;   //MOUSEEVENTF_LEFTDOWN 左键按下
	input.mi.time = 0;
	input.mi.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));

	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;   // MOUSEEVENTF_LEFTUP  左键松开
	input.mi.time = 0;
	input.mi.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
}

DWORD WINAPI LockThread(MultithreadLock* lock) {

	Process* m_process = lock->getProcessPtr();
	IParameter* m_parame = lock->getParamePtr();
	IPoint* m_point = lock->getIPointPtr();
	HANDLE lock_event = lock->getHandle();
	CustomizeMove MoveR = lock->getMoveRPtr();
	CustomizeMoveFree MoveClose = lock->getMoveClosePtr();

	Algorithm algorithm_x;
	Algorithm algorithm_y;
	std::vector<float> eucledian;		//! 用于存储欧式距离
	std::vector<int> indices;			//! 类别选择

	while (m_parame->aiStatus) {
		WaitForSingleObject(lock_event, INFINITE);
		eucledian.clear();
		indices.clear();
		IRect target_xywh;				//! 最近目标框

		for (size_t i = 0; i < m_process->indices.size(); i++) {
			if (m_parame->chooseSwitch) {
				switch (m_process->classes[m_process->indices[i]]) {
				case 0:
					chooseClass(eucledian, indices, m_process, m_point, i, m_parame->class0);
					break;
				case 1:
					chooseClass(eucledian, indices, m_process, m_point, i, m_parame->class1);
					break;
				case 2:
					chooseClass(eucledian, indices, m_process, m_point, i, m_parame->class2);
					break;
				case 3:
					chooseClass(eucledian, indices, m_process, m_point, i, m_parame->class3);
					break;
				}
			} else {
				float eucledian_temp = pow((m_point->origin_x + m_process->boxes[m_process->indices[i]].x) - m_point->center_x, 2) +
					pow((m_point->origin_y + m_process->boxes[m_process->indices[i]].y) - m_point->center_y, 2);
				eucledian.push_back(eucledian_temp);
			}
		}

		//! 最近的目标
		int idx = 0;
		if (m_parame->chooseSwitch) {
			if (indices.size()) continue;
			idx = indices[std::distance(std::begin(eucledian), std::min_element(std::begin(eucledian), std::end(eucledian)))];
		} else {
			idx = m_process->indices[std::distance(std::begin(eucledian), std::min_element(std::begin(eucledian), std::end(eucledian)))];
		}

		target_xywh.x = m_process->boxes[idx].x;
		if (m_parame->location) {				//! 偏移
			target_xywh.y = (m_process->boxes[idx].y + (m_process->boxes[idx].height * 0.5)) - (m_process->boxes[idx].height * m_parame->location);
		} else {
			target_xywh.y = m_process->boxes[idx].y;
		}

		target_xywh.y = m_process->boxes[idx].y;
		target_xywh.width = m_process->boxes[idx].width;
		target_xywh.height = m_process->boxes[idx].height;

		//! 相对距离
		float x = (m_point->origin_x + target_xywh.x) - m_point->center_x;
		float y = (m_point->origin_y + target_xywh.y) - m_point->center_y;

		//! 不在范围内
		if (abs(x) > m_parame->scops * 0.5 || abs(y) > m_parame->scops * 0.5) continue;

		//! 超前融合
		NULL;

		//! fov
		if (m_parame->fov_switch) {
			algorithm_x.FOVControl(&x, m_parame->hfov, m_parame->game_x_pixel, m_point->CapWidth, 360);	// point->CapWidth
			algorithm_x.FOVControl(&y, m_parame->vfov, m_parame->game_y_pixel, m_point->CapHeight, 180);
		}

		//! 控制算法
		if (m_parame->pid_switch) {
			algorithm_x.PidControl(&x, m_parame->P_x, m_parame->I_x, m_parame->D_x);
			algorithm_y.PidControl(&y, m_parame->P_y, m_parame->I_y, m_parame->D_y);
		}

		//! 最大移动像素
		if (m_parame->max_pixels) {
			algorithm_y.MaxMovePixel(&x, m_parame->max_pixels);
			algorithm_y.MaxMovePixel(&y, m_parame->max_pixels);
		}

		//! 自瞄逻辑
		//! 自动扳机
		if (m_parame->auto_fire) {
			if (!IGetAsyncKeyState(m_parame)) {
				sleep_start = std::chrono::system_clock::now();
			}

			if (IGetAsyncKeyState(m_parame)) {

				switch (m_parame->auto_model) {
				case 0:		// 先移动后开枪
					MoveR(int(x), int(y));
					if (((target_xywh.width * 0.5f) * m_parame->trigger_w) > abs(x) && ((target_xywh.height * 0.5f) * m_parame->trigger_h) > abs(y) && AutoComplete(m_parame, auto_start)) {
						if (GetAsyncKeyState(m_parame->auto_key)) {
							LeftButtonClick();
						}
						auto_start = std::chrono::system_clock::now();
					}
					break;
				case 1:		// 范围内开枪
					if ((target_xywh.width * 0.4f) > abs(x) && (target_xywh.height * 0.4f) > abs(y) && AutoComplete(m_parame, auto_start)) {
						if (GetAsyncKeyState(m_parame->auto_key)) {
							LeftButtonClick();
						}
						auto_start = std::chrono::system_clock::now();
					}
					break;
				case 2:		// 移动后延迟开枪
					MoveR(int(x), int(y));
					if (AutoComplete(m_parame, sleep_start)) {
						LeftButtonClick();
						sleep_start = std::chrono::system_clock::now();
					}
					break;
				case 3:

					break;
				}
			}
			continue;	//! 跳过，以免执行下面的单
		}

		//! 单移动
		if (m_parame->lock_model) {
			MoveR(int(x), int(y));
		} else {
			if (IGetAsyncKeyState(m_parame)) {
				MoveR(int(x), int(y));
			}
		}
	}

	lock->Release();
	return 0;
}


bool MultithreadLock::initLock() {
	//! 初始化事件
	CreateLockEvent();

	//! 初始化多线程
	lock_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LockThread, 0, 0, 0);
	if (lock_thread == NULL) {
		MessageBoxA(NULL, "创建Lock线程失败", "SF_62", MB_OK);
		return false;
	}
	return true;
}

//! 触发移动
void MultithreadLock::triggerMove() {
	SetEvent(lock_event);
}
	
//! 释放
void MultithreadLock::Release() {
	MoveClose();
	CloseHandle(lock_event);
	CloseHandle(lock_thread);
}

//! 获取处理结构体指针
Process* MultithreadLock::getProcessPtr() {
	return m_process;
}

//! 获取参数结构体指针
IParameter* MultithreadLock::getParamePtr() {
	return m_parame;
}

//! 获取参数结构体指针
IPoint* MultithreadLock::getIPointPtr() {
	return m_point;;
}

CustomizeMove MultithreadLock::getMoveRPtr() {
	return MoveR;
}

CustomizeMoveFree MultithreadLock::getMoveClosePtr() {
	return MoveClose;
}

HANDLE MultithreadLock::getHandle() {
	return lock_event;
}

bool MultithreadLock::CreateLockEvent() {
	char new_name[MAX_PATH]{};
	new_name[MAX_PATH - 1] = 0;
	_snprintf_s(new_name, 64, "%s%lu", "SF_TRT", (long)GetCurrentProcessId());
	lock_event = CreateEventA(NULL, false, false, new_name);
	if (lock_event == NULL) {
		MessageBoxA(NULL, "创建事件失败", "SF_62", MB_OK);
		return false;
	}
	return true;
}