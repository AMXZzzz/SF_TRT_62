#include <iostream>
#include <atltime.h>
#include "sf-trt.h"

#define EXPORT extern "C" __declspec(dllexport)

// ������
#define TRTHEAD "[DLL]: "
#define LOGINFO(format,...) if (getLogger()) {getLogger()->info(TRTHEAD ## format, __VA_ARGS__);} 
#define LOGWARN(format,...)  if (getLogger()) {getLogger()->warn(TRTHEAD ## format, __VA_ARGS__);}
#define LOGERROR(format,...) if (getLogger()) {getLogger()->error(TRTHEAD ## format, __VA_ARGS__);}

// bool start_thread(bool is_local);


sf::Type::YoloType convertYoloType(int type) {
	switch (type) {
	case 0: return sf::Type::YoloType::TYPE_YOLOV5;
	case 1: return sf::Type::YoloType::TYPE_YOLOV8;
	case 2: return sf::Type::YoloType::TYPE_YOLOX;
	}
}

sf::Type::FrameType convertBackend(int type) {
	switch (type) {
	case 0: return sf::Type::FrameType::TRT_FRAME;
	case 1: return sf::Type::FrameType::DML_FRAME;
	}
}

//! ��ʼ�������ڴ�
bool Intermediary::InitSharedParame() {

	HANDLE MapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SIZE, MapFileName);
	if (MapFile == NULL) {
		return false;
	}

	parame = (IParameter*)MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE);
	if (parame == NULL) {
		return false;
	}

	//! ------------- �������� -------------
	parame->conf = 0.3;
	parame->iou = 0.1;
	parame->showWindows = true;
	parame->module_path = "clbq_yolov5s_1w_640.engine";
	parame->yolo_type = 0;
	parame->backend = 0;
	parame->equipment = 0;

	return true;
}

bool Intermediary::InitLogger() {
	// ��ʼ����־
	CTime t = CTime::GetCurrentTime();
	static char temp[MAX_PATH]{};
	_snprintf_s(temp, MAX_PATH, "logs/%d-%d-%d %d-%d-%d.txt", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
	g_logger = spdlog::basic_logger_mt("SF_TRT", temp);
	g_logger->flush_on(spdlog::level::trace);

	return true;
}

//! ��ʼ��yolo
bool Intermediary::InitYoloTable() {
	//! ���ָ��
	if (&parame->conf == nullptr || &parame->iou == nullptr) {
		LOGINFO("����ָ��Ϊnullptr")
		return false;
	}
	YOLOINFO info{};
	info.type = convertYoloType(parame->yolo_type);
	info.conf = &parame->conf;
	info.iou = &parame->iou;
	info.process = &process;
	yolo = sf::createYoloTable(&info);
	return true;
}

//! ��ʼ��frame
bool Intermediary::InitFrame() {
	//! ���ָ��
	if (yolo == nullptr) {
		LOGWARN("yoloָ��Ϊnullptr")
		return false;
	}
	FRAMEINFO frame_info{};
	frame_info.frame_type = convertBackend(parame->backend);
	frame_info.yolo = yolo;
	frame_info.logger = getLogger();
	frame_info.equipment = parame->equipment;
	frame = sf::CreateFrame(&frame_info);
	return true;
}

//! ��ʼ��ģ��
bool Intermediary::InitModel() {
	if (!frame->AnalyticalModel(parame->module_path)) {
		std::cout << frame->getLastErrorInfo().getErrorInfo() << std::endl;
		LOGWARN("����ģ�ʹ���:{}", frame->getLastErrorInfo().getErrorInfo().c_str())
		return false;
	}
	return true;
}

//! ��ʼ���߼�
bool Intermediary::InitLock() {
	LOCKINFO lock_info{};
	lock_info.parame = parame;
	lock_info.process = &process;
	lock_info.point = &point;
	lock_info.logger = getLogger();
	lock = sf::crateLock(&lock_info);

	if (!lock->initLock()) {
		LOGWARN("��ʼ���ƶ��߼�ʧ��");
		return false;
	}

	return true;
}

//! ��ʼ��DX��ͼ
bool Intermediary::InitDX() {
	dxgi = sf::createDxgi();
	if (dxgi == nullptr) {
		return false;
	}
	//! ��ʼ����ͼ��Ϣ
	dxgi->DXGIInitPont(&point);
	//! ��ʼ��DX��ͼ��Χ
	dxgi->CaptureResource(yolo->getImageSize().width, yolo->getImageSize().height);
	return true;
}

//! ����
bool Intermediary::Detection() {
	cv::Mat img;
	//! ��λ�ź�
	parame->aiStatus = true;		//! ai����״̬

	while (!parame->uiSendStop) {
		//! ��ͼ
		dxgi->BitmapToMat(&img);
		//! ����
		frame->Detect(img);
		//! ����
		lock->triggerMove();
		//! ����
		DrawBox(img);
	}

	//! ��λ�ź�
	parame->aiStatus = false;

	//! �رմ���
	if (cv::getWindowProperty(OPENCV_WINDOWS_NAME, cv::WND_PROP_VISIBLE))
		cv::destroyWindow(OPENCV_WINDOWS_NAME);
	return true;
}

//! ���ƿ�
void Intermediary::DrawBox(cv::Mat& img) {
	//! ���ƿ�
	if (parame->showWindows) {
		for (size_t i = 0; i < process.indices.size(); ++i) {
			cv::rectangle(img, cv::Rect(process.boxes[int(process.indices[i])].x - (process.boxes[int(process.indices[i])].width * 0.5f),
				process.boxes[int(process.indices[i])].y - (process.boxes[int(process.indices[i])].height * 0.5f),
				process.boxes[int(process.indices[i])].width, process.boxes[int(process.indices[i])].height),
				cv::Scalar(0, 255, 0), 2, 8, 0);
		}
		cv::imshow(OPENCV_WINDOWS_NAME, img);
		cv::waitKey(1);
	} else {
		if (cv::getWindowProperty(OPENCV_WINDOWS_NAME, cv::WND_PROP_VISIBLE))
			cv::destroyWindow(OPENCV_WINDOWS_NAME);
	}
}

//! �ͷ�
void Intermediary::Release() {
	//! �ͷ�DX
	if (dxgi) {
		dxgi->Release();
		dxgi = nullptr;
	}
	//! �ͷ�frame
	if (frame) {
		frame->Release();
		frame = nullptr;
	}
	//! �ͷ�yolo
	if (yolo) {
		yolo->Release();
		yolo = nullptr;
	}
	//! �ͷ�lock
	if (lock) {
		lock->Release();
		lock = nullptr;
	}
	delete this;
}

//! ��ȡ��־����
std::shared_ptr<spdlog::logger> Intermediary::getLogger() {
	return g_logger;
}

/*
	���±�
	- �޸�ִ����ΪDLL
	- ��������ģ��
	- ���Ӳ����ں���
*/


//! ����C���� ���߳�
EXPORT bool start_thread(bool is_local = true) {

	Intermediary* intermediary  = new Intermediary();

	//! ��ʼ�������ڴ�
	if (!intermediary->InitSharedParame())  goto Free;
	//! ��ʼ����־, �ȳ�ʼ�������ڴ�
	if (is_local) {
		intermediary->InitLogger();
	}
	//! ��ʼ�����ñ�
	if (!intermediary->InitYoloTable())	goto Free;
	//! ��ʼ��frame
	if (!intermediary->InitFrame())	goto Free;
	//! ��ʼ��ģ��
	if (!intermediary->InitModel())	goto Free;
	//! ��ʼ��DX��ͼ
	if (!intermediary->InitDX()) goto Free;
	//! ��ʼ������
	if (!intermediary->InitLock()) goto Free;
	//! ѭ������
	intermediary->Detection();

Free:
	//! �ͷ�
	intermediary->Release();

	return true;
}

//! ��ں���
#if 1 // test
int main() {
	std::cout << " test" << std::endl;
	start_thread(true);

	return 0;
}
#endif