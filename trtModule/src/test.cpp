#include <spdlog/sinks/basic_file_sink.h>
#include <atltime.h>
#include "trt-module.h"


std::shared_ptr<spdlog::logger> g_logger;

void initlog() {
	// 初始化日志
	CTime t = CTime::GetCurrentTime();
	static char temp[MAX_PATH]{};
	_snprintf_s(temp, MAX_PATH, "logs/%d-%d-%d %d-%d-%d.txt", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
	g_logger = spdlog::basic_logger_mt("SF_TRT", temp);
	g_logger->flush_on(spdlog::level::trace);
}

int main() {
	initlog();

	char* module_path = "clbq_yolov5s_1w_640.engine";
	float conf = 0.3;
	float iou = 0.1;
	Process process;

	YOLOINFO info{};
	info.type = sf::Type::TYPE_YOLOV5;
	info.conf = &conf;
	info.iou = &iou;
	info.process = &process;

	YOLO* yolo = sf::createYoloTable(&info);

	Frame* frame = new ITensorRt(yolo, g_logger, 0);
	if (!frame->AnalyticalModel(module_path)) {
		std::cout << frame->getLastErrorInfo().getErrorInfo() << std::endl;
	}

	std::cout << frame->getLastErrorInfo().getErrorInfo() << std::endl;


	frame->Release();

	return 0;
}