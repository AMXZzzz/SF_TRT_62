#include <yolo.h>

#define MAX_SIZE 256
#define MapFileName L"SF_TRT_62"

struct Parameter {
	//! 需要共享的参数
	float conf = 0.3;						//！ 置信度指针
	float iou = 0.1;						//！ iou指针
	int equipment = 0;						//！ 设备
	int yolo_type = 0;						//！ yolo类型
	int backend = 0;						//！ 后端类型
	std::string module_path;					//！ 模型路径
	//std::shared_ptr<spdlog::logger> g_logger;	//！ 日志智能指针

	//! 信号量
	bool uiSendStop = false;		//！ ui发送停止信号， true 为发送了停止信号
	bool aiStatus = false;			//！ ai运行状态指针, true 为运行中，false 为停止
	bool showWindows = false;		//！ 是否显示窗口,true 为显示，false 为不显示
};
