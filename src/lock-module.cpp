
#include "lock-module.h"
#include "move-way.h"
#include "parameter.h"
#include <SFDXGI.h>


static VOID PushIndices(std::vector<float>& EuclideanDistance, 
	std::vector<int>& indices, 
	Process* process,
	const int& idx ,
	const bool& classes){
	SFPoint* point = &SF_DXGI::Get().point;
	if (classes) {
		indices.push_back(process->_indices[idx]);
		EuclideanDistance.push_back(
			pow((point->origin_x + process->_boxes[process->_indices[idx]].x) - point->center_x, 2) +
			pow((point->origin_y + process->_boxes[process->_indices[idx]].y) - point->center_y, 2)
		);
	}
}

static inline BOOL IGetAsyncKeyState(Parameter* param) {
	if (param->lock_key2 == 0)
		return GetAsyncKeyState(param->lock_key);
	else
		return GetAsyncKeyState(param->lock_key) || GetAsyncKeyState(param->lock_key2);
}

static std::chrono::system_clock::time_point auto_start = std::chrono::system_clock::now();
static inline BOOL AutoComplete(Parameter* param) {
	int random_time = 0;
	if (param->auto_random != 0) {
		random_time = (int)param->auto_random * rand() / (RAND_MAX + 1);
	}
	std::chrono::system_clock::time_point auto_end = std::chrono::system_clock::now();
	return std::chrono::duration_cast <std::chrono::milliseconds> (auto_end - auto_start).count() >= (param->auto_interval + random_time) * 1.0;
}

static inline VOID LeftButtonClick() {
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
	auto_start = std::chrono::system_clock::now();
}

BOOL LockMode::InitLock(int way) {
	// 初始化移动
	BOOL init = MoveWay::Get().InitMove(way);
    return !init;
}

VOID LockMode::StratLock(Parameter* param){
	LockMode* lock = &LockMode::Get();
	MoveWay* moveway = &MoveWay::Get();
	SFPoint* point = &SF_DXGI::Get().point;
	Algorithm algorithm_x;
	Algorithm algorithm_y;

	std::vector<float> EuclideanDistance;
	std::vector<int> indices;

	Process* process = &param->process;

	if (!param->executionStatus ||! &param->uiStop) {
		return;
	}
	if (!process->_indices.size()) {return;};

	cv::Rect target_xywh;
	bool category = param->ModelClasses;
	EuclideanDistance.clear();
	indices.clear();
	for (int i = 0; i < process->_indices.size(); i++) {

		if (category) {
			switch (process->_classes[process->_indices[i]]) {
			case 0:
				PushIndices(EuclideanDistance, indices, process, i, param->class0);
				break;
			case 1:
				PushIndices(EuclideanDistance, indices, process, i, param->class1);
				break;
			case 2:
				PushIndices(EuclideanDistance, indices, process, i, param->class2);
				break;
			case 3:
				PushIndices(EuclideanDistance, indices, process, i, param->class3);
				break;
			}
		}else {
			EuclideanDistance.push_back(
				pow((point->origin_x + process->_boxes[process->_indices[i]].x) - point->center_x, 2) +
				pow((point->origin_y + process->_boxes[process->_indices[i]].y) - point->center_y, 2)
			);
		}
	}

	// 最近目标索引
	int idx = 0;
	if (category) {
		if (!indices.size())
			return;
		idx = indices[std::distance(std::begin(EuclideanDistance), std::min_element(std::begin(EuclideanDistance), std::end(EuclideanDistance)))];
	}
	else {
		idx = process->_indices[std::distance(std::begin(EuclideanDistance), std::min_element(std::begin(EuclideanDistance), std::end(EuclideanDistance)))];
	}
	target_xywh.x = process->_boxes[idx].x;
	target_xywh.y = process->_boxes[idx].y;
	target_xywh.width = process->_boxes[idx].width;
	target_xywh.height = process->_boxes[idx].height;

	// 偏移位置
	if (param->location)
		target_xywh.y = (target_xywh.y + (target_xywh.height * 0.5)) - (target_xywh.height * param->location);

	// 相对距离
	float x = (point->origin_x + target_xywh.x) - point->center_x;
	float y = (point->origin_y + target_xywh.y) - point->center_y;
	// 范围外跳过 
	if (abs(x) > param->effectiverange * 0.5 || abs(y) > param->effectiverange * 0.5) return;

	// FOV控制计算 
	if (param->fov_algorithm) {
		algorithm_x.HFOVControl(&x, param->game_x_pixel, point->CapWidth);	// point->CapWidth
		algorithm_y.VFOVControl(&y, param->game_y_pixel, point->CapHeight);
	}

	// PID控制 减少内存复制
	algorithm_x.PidControl(&x, param->P_x, param->I_x, param->D_x);
	algorithm_y.PidControl(&y, param->P_y, param->I_y, param->D_y);

	// 最大移动
	algorithm_x.MaxMovePixel(&x, param->max_pixels);
	algorithm_y.MaxMovePixel(&y, param->max_pixels);

	if (param->auto_fire) {
		if (IGetAsyncKeyState(param) && param->AimSwitch) {
			if (param->auto_model == 0) {
				// 先移动后开枪
				//moveway->MoveR((int)x, int(y));
				if ((target_xywh.width * 0.4f) > abs(x) && (target_xywh.height * 0.4f) > abs(y) && AutoComplete(param)) {
					// 左键按下并进入冷却
					LeftButtonClick();
				}
			}
			else {
				if ((target_xywh.width * 0.4f) > abs(x) && (target_xywh.height * 0.4f) > abs(y) && AutoComplete(param)) {
					// 左键按下并进入冷却
					LeftButtonClick();
				}
			}
		}
		return;
	}
	// 单移动
	if (IGetAsyncKeyState(param) && param->AimSwitch) {
		moveway->MoveR((int)x, int(y));
	}
}

VOID LockMode::Release()
{
  
}
