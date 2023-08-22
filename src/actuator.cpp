#include "actuator.h"
#include "trt-module.h"
#include "dml-module.h"
#include "imgui-module.h"
#include "lock-module.h"
#include <SFDXGI.h>

DWORD WINAPI IThread(void* args) {

	Actuator* actuator = static_cast<Actuator*> (args);
	Parameter* parame = actuator->getParameterPtr();
	YOLO* yolo = actuator->getYoloConfig();
	Frame* frame = actuator->getFrame();
	LockMode* lock = &LockMode::Get();
	lock->InitLock(parame->move_way);
	// ��ʼ����ͼ			 ��������ع�
	SF_DXGI* sf = &SF_DXGI::Get();

	sf->CaptureResource(yolo->getInputDims()[2], yolo->getInputDims()[3]);
	cv::Mat img;
	while (parame->executionStatus && parame->ai_is_run) {
		//! ��ͼ
		sf->BitmapToMat(&img);

		//! ����
		frame->Detect(img);

	
		//! ���麯��
		lock->StratLock(parame);
		//fire ->AimfFire(frame)
	}
	//! �ͷ�,ֻ���ͷ� sf ,yolo ,frame
	if (cv::getWindowProperty(WINDOWS_NAME, cv::WND_PROP_VISIBLE))
		cv::destroyWindow(WINDOWS_NAME);
	lock->Release();
	frame->Release();
	sf->Release();
	yolo->Release();
	//fire->Release();

	//! ��λ
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
	//! ��λ,��ֹ��δ����߳�
	_parame->ai_is_run = true;

	//! ��ʼ��yolo���ñ�
	YoloFactory* factory = YoloFactory::createYoloFactory();
	factory->AcquireYoloType(convertYoloType(yolotype));			// ���ù�������
	factory->AcquireConfidencePtr(&_parame->conf);
	factory->AcquireIOUPtr(&_parame->iou);
	factory->AcquireProcessPtr(&_parame->process);
	factory->AcquireShowWindowPtr(&_parame->showWindow);
	IStates hr = factory->QueryInterface(reinterpret_cast<void**>(&_yolo));
	if (hr.is_error()) {
		std::cout << "������Ϣ:" << hr.msg() << std::endl;
		_parame->ai_is_run = false;
		return false;
	}
	factory->Release();		// �������꼴���ͷ�

	//! ��ʼ�����
	//FrameFactory* factory_fame = convertBackendType(backend);	// trt
	FrameFactory* factory_fame = sf::trt::TRTFactory::createTRTFactory();	// trt
	factory_fame->AcquireYoloPtr(_yolo);			// ���ù�������
	factory_fame->AcquireLoggerPtr(_logger);
	factory_fame->AcquireEquipmen(0);
	hr = factory_fame->QueryInterface(reinterpret_cast<void**>(&_frame));
	if (hr.is_error()) {
		std::cout << "������Ϣ:" << hr.msg() << std::endl;
		_parame->ai_is_run = false;
		return false;
	}
	factory_fame->Release();

	// ����ģ��
	if (!_frame->AnalyticalModel(model_path)) {
		std::cout << "������:" << _frame->getLastErrorInfo().err_code() << "  ���Ĵ�����Ϣ:" << _frame->getLastErrorInfo().msg() << std::endl;
		_parame->ai_is_run = false;
		return false;
	}

	thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)IThread, this, 0, 0);
	if (thread == NULL) {
		_parame->ai_is_run = false;
		_logger->warn("����AI�߳�ʧ��");
		MESSAGEBOXA("����Ai�߳�ʧ��", MB_OK);
	}
	return true;
}

Parameter* Actuator::getParameterPtr() {
	return _parame;
}

std::shared_ptr<spdlog::logger> Actuator::getLoggerPtr()
{
	return std::shared_ptr<spdlog::logger>();
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
