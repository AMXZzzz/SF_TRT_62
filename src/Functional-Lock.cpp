#include <windows.h>
#include "Functional-Lock.h"


//void chooseClass(std::vector<float>& EuclideanDistance,
//	std::vector<int>& indices,
//	Process* process,
//	IPoint* point,
//	const int& idx,
//	const bool& classes) {
//	if (classes) {
//		indices.push_back(process->indices[idx]);
//		EuclideanDistance.push_back(
//			pow((point->origin_x + process->boxes[process->indices[idx]].x) - point->center_x, 2) +
//			pow((point->origin_y + process->boxes[process->indices[idx]].y) - point->center_y, 2)
//		);
//	}
//}

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

bool FunctionalLock::initLock() {

	return false;
}

void FunctionalLock::triggerMove() {

	std::vector<float> eucledian;		//! 用于存储欧式距离
	std::vector<int> indices;			//! 类别选择
	IRect target_xywh;				//! 最近目标框
	for (size_t i = 0; i < m_process->indices.size(); i++) {
		if (m_parame->chooseSwitch) {
			switch (m_process->classes[m_process->indices[i]]) {
			case 0:
				chooseClass(eucledian, indices, m_process, m_point,i, m_parame->class0);
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
		if (indices.size()) return;
		idx = indices[std::distance(std::begin(eucledian), std::min_element(std::begin(eucledian), std::end(eucledian)))];
	} else {
		idx = m_process->indices[std::distance(std::begin(eucledian), std::min_element(std::begin(eucledian), std::end(eucledian)))];
	}

	target_xywh.x = m_process->boxes[idx].x;
	if (m_parame->location){				//! 偏移
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
	if (abs(x) > m_parame->scops * 0.5 || abs(y) > m_parame->scops * 0.5) return;

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

void FunctionalLock::Release() {
	MoveClose();
}
