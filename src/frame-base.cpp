#include "frame-base.h"


IStates FrameFactory::AcquireYoloPtr(YOLO* yolo) {
	_yolo = yolo;
	return IStates();
}

IStates FrameFactory::AcquireLoggerPtr(std::shared_ptr<spdlog::logger> logger) {
	_logger = logger;
	return IStates();
}

IStates FrameFactory::AcquireEquipmen(int equipment) {
	_equipment = equipment;
	return IStates();
}

IStates FrameFactory::checkFrameMemberPtr() {
	if (!_yolo) return IStates(State::UNKONEERR, "yolo配置表无效");
	if (!_logger) return IStates(State::UNKONEERR, "日志无效");
	return IStates();
}
