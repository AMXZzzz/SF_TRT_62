#pragma once
#include "factory-base.h"

struct Parameter {

	//! yolo类型
	int yolo_type = 0;
	//! 后端
	int backend = 0;
	//! 模型路径
	char model_path[MAX_PATH] = "";

	float conf = 0.3;
	float iou = 0.1;
	Process process;
	bool uiStop = true;
	bool executionStatus = true;
	//！ 显示推理窗口
	bool showWindow = false;

	bool ai_is_run = false;

	bool key_ui = true;		     // 按键UI
	bool parame_ui = false;		// 显示参数UI
	bool pid_ui = false;			// PIDui
	bool fov_algorithm = false;			// fov ui


	bool AimSwitch = true;			// 自瞄状态
	int move_way = 0;    //移动方式
	bool ModelClasses = false;		// 类别选择

	float location = 0.75;				// 偏移
	int effectiverange = 200;			// 自瞄有效范围
	int max_pixels = 15;				// 限制移动距离
	int class_number = 1;				// 类别数量
	int max_range = 640;			// 最大范围

	bool class0 = false;
	bool class1 = false;
	bool class2 = false;
	bool class3 = false;

	float P_x = 0.3;
	float I_x = 0.1;
	float D_x = 0.1;
	float P_y = 0.3;
	float I_y = 0.1;
	float D_y = 0.1;

	int game_x_pixel = 0;		// 横向像素
	int game_y_pixel = 0;		// 横向像素

	int lock_key = 0x02;		// 触法按键
	int lock_key2 = 0;			// 自定义触法按键

	bool auto_fire = false;		// 开启自动开火
	int auto_interval = 150;	// 间隔150ms
	int auto_random = 80;		// 加上的随机数
	int auto_model = 0;			// 扳机模式

	//! 是共享内存
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
	HANDLE _MapFile = NULL;		// 记录Map指针,由自身释放

};

class ParameFactory final : public IFactory {
public:
	static ParameFactory* createParameFactory() {
		return new ParameFactory();
	}

	//! 获取对象
	IStates QueryInterface(void** parme) override;

	//! 设置为共享内存
	void setSharedMemory();

	//! 释放
	void Release() override;

private:
	HANDLE _MapFile;
	bool is_sharedmemory = false;

	ParameFactory() {}
};
