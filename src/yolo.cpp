#include "yolo.h"
#include "yolov5.h"
#include "yolov8.h"
#include "yolox.h"


IStates YoloFactory::QueryInterface(void** yolox) {
	IStates hr = checkMemberPtr();
	if (hr.err_code() != State::Success) {
		return hr;
	}
	switch (_type) {
	case sf::Type::TYPE_YOLOV5:
		*yolox = new sf::Pattern::YOLOV5(_conf, _iou, _showWindow, _process);
		break;
	case sf::Type::TYPE_YOLOV8:
		*yolox = new sf::Pattern::YOLOV8(_conf, _iou, _showWindow, _process);
		break;
	case sf::Type::TYPE_YOLOX:
		*yolox = new sf::Pattern::YOLOX(_conf, _iou, _showWindow, _process);
		break;
	}
	return IStates();
}

IStates YoloFactory::AcquireYoloType(sf::Type::YoloType type) {
	_type = type;
	return IStates();
}

IStates YoloFactory::AcquireProcessPtr(Process* process) {
	if (!process) {
		return IStates(State::UNKONEERR, "process ָ����Ч");
	}
	_process = process;
	return IStates();
}
IStates YoloFactory::AcquireConfidencePtr(float* conf) {

	if (!conf) {
		return IStates(State::UNKONEERR, "����conf ����");
	}
	_conf = conf;
	return IStates();
}
IStates YoloFactory::AcquireIOUPtr(float* iou) {
	if (!iou) {
		return IStates(State::UNKONEERR, "����iou ����");
	}
	_iou = iou;
	return IStates();
}

IStates YoloFactory::AcquireShowWindowPtr(bool* showWindow) {
	_showWindow = showWindow;
	return IStates();
}
void YoloFactory::Release() {
	delete this;
}
IStates YoloFactory::checkMemberPtr() {
	if (!_conf) return IStates(State::CONF_EMPTY, "confָ����Ч,��ָ����Ч����");
	if (!_iou) return IStates(State::IOU_EMPTY, "iouָ����Ч");
	if (!_showWindow) return IStates(State::ShowWindow_EMPTY, "iouָ����Ч");
	if (!_process) return IStates(State::PRE_EMPTY, "processָ����Ч");
	if (_type == sf::Type::YoloType::TYPE_UNKONE) return IStates(State::UNKONEERR, "δ����yolo���õ�����");
	return IStates(State::Success, "����ָ����Ч");
}
