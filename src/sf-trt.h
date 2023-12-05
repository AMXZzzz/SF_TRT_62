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

//! 跨进程数据指针,实时性
struct Data {
	float conf = 0.3;							//! 置信度指针
	float iou = 0.1;							//! iou置信度指针
	float aim_position = 0.7;					//! 瞄准位置
	int aim_range = 200;						//! 自瞄范围,以屏幕中间为中心的 aim_range * aim_range正方形
	int aim_key = 0x01;							//! 自瞄触发键
	int aim_key2 = 0x01;						//! 自瞄触发键2
	int aim_key3 = 0x02;						//! 自瞄触发键3
	int auto_model = 0;							//! 扳机方式
	int auto_key = 0;							//! 扳机按键 key
	int auto_ke2 = 0;							//! 扳机按键 key2
	int auto_ke3 = 0;							//! 扳机按键 key3
	float auto_trigger_x_scale = 0.5;			//! 自动扳机左右触发范围比例
	float auto_trigger_up_scale = 0.5;			//! 自动扳机上部触发范围比例
	float auto_trigger_down_scale = 0.5;		//! 自动扳机下部触发范围比例
};

//! 跨进程配置信息，配置性
struct Info {
	int yolo_tyoe = 0;						//! yolo类型 0是yolov5, 1是yolov8, 2是yolox
	int frame_type = 0;						//! 框架类型 0是tensrort, 1是dml
	int equipment = 0;						//! 设备索引 
	int lock_type = 0;						//! 自瞄逻辑的方式 
	int mousec_type = 0;					//! 移动方式 
	std::string model_path = "";			//! 模型路径
	std::string logger_path = "";			//! 模型路径

};

//! 跨进程信号，实时性
struct Signal {
	bool dll_exit_signal = false;			//! 当置为true时，整个dll退出信号
	bool ai_start_signal = false;			//! 当为true时，启动ai线程
	bool show_detect_window = false;		//! 显示窗口
	bool category_filter = false;			//! 类别筛选
	bool first_class = false;				//! 第一个类别
	bool second_class = false;				//! 第二个类别
	bool third_class = false;				//! 第三个类别
	bool fourth_class = false;				//! 第四个类别
	bool auto_trigger = false;				//! 自动扳机开关
	bool keep_move = false;					//! 持续自瞄
};

struct SharedMemory {
	Data s_data;
	Signal s_signal;
	Info s_info;
};
