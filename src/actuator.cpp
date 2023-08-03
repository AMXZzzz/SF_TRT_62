#include "actuator.h"
#include <SFDXGI.h>
#include "trt-module.h"
#include "dml-module.h"


DWORD WINAPI IThread(void* args) {

	Actuator* actuator = static_cast<Actuator*> (args);
	Parameter* parame = actuator->getParameterPtr();
	YOLO* yolo = actuator->getYoloConfig();
	Frame* frame = actuator->getFrame();


	// 初始化截图			 面向对象重构
	SF_DXGI* sf = &SF_DXGI::Get();
	sf->CaptureResource(yolo->getInputDims()[2], yolo->getInputDims()[3]);

	cv::Mat img;
	while (parame->executionStatus && parame->ai_is_run) {
		//! 截图
		sf->BitmapToMat(&img);

		//! 推理
		frame->Detect(img);

		//! 自瞄函数
	}

	//! 释放,只能释放 sf ,yolo ,frame
	if (cv::getWindowProperty(WINDOWS_NAME, cv::WND_PROP_VISIBLE))
		cv::destroyWindow(WINDOWS_NAME);
	frame->Release();
	sf->Release();
	yolo->Release();

	//! 复位
	parame->executionStatus = true;
	parame->ai_is_run = false;

	return 0;
}

sf::Type::YoloType convertYoloType(int type) {
	switch (type) {
	case 0: return sf::Type::TYPE_YOLOV5;
	case 1: return sf::Type::TYPE_YOLOV8;
	case 2: return sf::Type::TYPE_YOLOX;
	}
}

FrameFactory* convertBackendType(int backend) {
	switch (backend) {
	case 0: return sf::trt::TRTFactory::createTRTFactory();
	case 1: return sf::dml::DMLFactory::createDMLFactory();
	}
}

bool Actuator::executionThread(int yolotype, int backend, char* model_path) {
	//! 置位,防止多次创建线程
	_parame->ai_is_run = true;

	//! 初始化yolo配置表
	YoloFactory* factory = YoloFactory::createYoloFactory();
	factory->AcquireYoloType(convertYoloType(yolotype));			// 设置工厂属性
	factory->AcquireConfidencePtr(&_parame->conf);
	factory->AcquireIOUPtr(&_parame->iou);
	factory->AcquireProcessPtr(&_parame->process);
	IStates hr = factory->QueryInterface(reinterpret_cast<void**>(&_yolo));
	if (hr.is_error()) {
		std::cout << "错误信息:" << hr.msg() << std::endl;
		_parame->ai_is_run = false;
		return false;
	}
	factory->Release();		// 工厂用完即可释放

	//! 初始化后端
	//FrameFactory* factory_fame = convertBackendType(backend);	// trt
	FrameFactory* factory_fame = sf::trt::TRTFactory::createTRTFactory();	// trt
	factory_fame->AcquireYoloPtr(_yolo);			// 设置工厂属性
	factory_fame->AcquireLoggerPtr(_logger);
	factory_fame->AcquireEquipmen(0);
	hr = factory_fame->QueryInterface(reinterpret_cast<void**>(&_frame));
	if (hr.is_error()) {
		std::cout << "错误信息:" << hr.msg() << std::endl;
		_parame->ai_is_run = false;
		return false;
	}
	factory_fame->Release();

	// 加载模型
	if (!_frame->AnalyticalModel(model_path)) {
		std::cout << "错误码:" << _frame->getLastErrorInfo().err_code() << "  最后的错误消息:" << _frame->getLastErrorInfo().msg() << std::endl;
		_parame->ai_is_run = false;
		return false;
	}

	thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)IThread, this, 0, 0);
	if (thread == NULL) {
		_parame->ai_is_run = false;
		_logger->warn("创建AI线程失败");
		MESSAGEBOXA("创建Ai线程失败", MB_OK);
	}
	return true;
}

Parameter* Actuator::getParameterPtr() {
	return _parame;
}

YOLO* Actuator::getYoloConfig() {
	return _yolo;
}

Frame* Actuator::getFrame() {
	return _frame;
}

IStates actuatorFactory::QueryInterface(void** actuator) {
	*actuator = new Actuator(_parame, _logger);
	return IStates();
}

IStates actuatorFactory::AcquireLoggerPtr(std::shared_ptr<spdlog::logger> logger) {
	_logger = logger;
	return IStates();
}

IStates actuatorFactory::AcquireParamePtr(Parameter* parame) {
	_parame = parame;
	return IStates();
}
