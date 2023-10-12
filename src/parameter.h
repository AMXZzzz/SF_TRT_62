#pragma once
#include <string>
#include "parameter.h"
#include "yolo.h"

#define MAX_SIZE 256
#define MapFileName L"SF_TRT_62"

typedef bool(*CustomizeConitor)(int);

struct IParameter {
	//! 需要共享的参数
	float conf = 0.3;						//！ 置信度指针
	float iou = 0.1;						//！ iou指针
	int equipment = 0;						//！ 设备
	int yolo_type = 0;						//！ yolo类型
	int backend = 0;						//！ 后端类型
	int lock_method = 0;					//!  移动逻辑方式， 0： 函数式，1： 多线程
	std::string module_path;				//！ 模型路径
	CustomizeConitor conitor;				//！ 自定义监听函数		

	//! 用于控制的参数
	bool pid_switch = false;		//！ pid开关，true 为开，false 为关
	float P_x = 0.3f;				//！ x轴PID参数
	float I_x = 0.1f;
	float D_x = 0.1f;
	float P_y = 0.3f;				//！ y轴PID参数
	float I_y = 0.1f;
	float D_y = 0.1f;

	//! fov
	bool fov_switch = false;		//！ fov开关，true 为开，false 为关
	float vfov = 0.1f;				//！ 实际vfov参数
	float hfov = 0.1f;				//!  实际hfov参数
	int game_x_pixel = 0;			//！ 游戏x轴像素	
	int game_y_pixel = 0;			//！ 游戏y轴像素

	//！ 最大像素
	int max_pixels = 100;

	//！ 自动扳机
	bool auto_fire = false;			//！ 自动扳机开关，true 为开，false 为关
	int auto_random = 0;			//！ 自动扳机随机数
	int auto_interval = 0;			//！ 自动扳机间隔
	int auto_model = 0;				//!  扳机模式
	float trigger_w = 0.75f;		//!  扳机触发范围
	float trigger_h = 0.75f;		//!  扳机触发范围
	unsigned char auto_key = 0x01;			//!  扳机触发按键

	//! 自瞄方式
	int lock_model = 0;				//！ 自瞄方式
	float location = 0.f;			//！ 瞄准位置
	int scops = 0;					//！ 瞄准范围

	//! 按键
	bool lock_switch = false;		//！ 自瞄开关，true 为开，false 为关
	int lock_key = 0;				//！ 自瞄按键
	int lock_key2 = 0;				//！ 自瞄按键2
	int monitor_module = 0;			//！ 监听方式  0：API监听 1：硬件监听

	//! 信号量
	bool uiSendStop = false;		//！ ui发送停止信号， true 为发送了停止信号
	bool aiStatus = false;			//！ ai运行状态指针, true 为运行中，false 为停止
	bool showWindows = false;		//！ 是否显示窗口,true 为显示，false 为不显示
	bool chooseSwitch = false;		//！ 类别选择开关，true 为开，false 为关
	bool class0 = false;			//！ 类别0选择开关，true 为开，false 为关
	bool class1 = false;			//！ 类别1选择开关，true 为开，false 为关
	bool class2 = false;			//！ 类别2选择开关，true 为开，false 为关
	bool class3 = false;			//！ 类别3选择开关，true 为开，false 为关

};