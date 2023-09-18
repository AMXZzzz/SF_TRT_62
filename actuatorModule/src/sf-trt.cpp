#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include <atltime.h>

#include "yolo.h"
#include "frame.h"
#include "sf-trt.h"

#define EXPORT extern "C" __declspec(dllexport)

// ������
#define TRTHEAD "[DLL]: "
#define LOGINFO(format,...) if (getLogger()) {getLogger()->info(TRTHEAD ## format, __VA_ARGS__);} 
#define LOGWARN(format,...)  if (getLogger()) {getLogger()->warn(TRTHEAD ## format, __VA_ARGS__);}
#define LOGERROR(format,...) if (getLogger()) {getLogger()->error(TRTHEAD ## format, __VA_ARGS__);}

bool start_thread(bool is_local);


sf::Type::YoloType convertYoloType(int type) {
	switch (type) {
	case 0: return sf::Type::TYPE_YOLOV5;
	case 1: return sf::Type::TYPE_YOLOV8;
	case 2: return sf::Type::TYPE_YOLOX;
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

	parme = (Parameter*)MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE);
	if (parme == NULL) {
		return false;
	}

	//! test��ʼ������
	parme->conf = 0.3;
	parme->iou = 0.1;
	parme->showWindows = true;
	parme->module_path = "clbq_yolov5s_1w_640.engine";
	parme->yolo_type = 0;
	parme->backend = 0;
	parme->equipment = 0;

	return true;
}

bool Intermediary::InitLogger() {
	// ��ʼ����־
	CTime t = CTime::GetCurrentTime();
	static char temp[MAX_PATH]{};
	_snprintf_s(temp, MAX_PATH, "logs/%d-%d-%d %d-%d-%d.txt", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
	g_logger = spdlog::basic_logger_mt("SF_TRT", temp);
	g_logger->flush_on(spdlog::level::trace);
}

//! ��ʼ��yolo
bool Intermediary::InitYoloTable() {
	//! ���ָ��
	if (&parme->conf == nullptr || &parme->iou == nullptr) {
		LOGINFO("����ָ��Ϊnullptr")
		return false;
	}
	YOLOINFO info{};
	info.type = convertYoloType(parme->yolo_type);
	info.conf = &parme->conf;
	info.iou = &parme->iou;
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
	frame_info.frame_type = convertBackend(parme->backend);
	frame_info.yolo = yolo;
	frame_info.logger = getLogger();
	frame_info.equipment = parme->equipment;
	frame = sf::CreateFrame(&frame_info);
	return true;
}

//! ��ʼ��ģ��
bool Intermediary::InitModel() {
	if (!frame->AnalyticalModel(parme->module_path)) {
		std::cout << frame->getLastErrorInfo().getErrorInfo() << std::endl;
		LOGWARN("����ģ�ʹ���:{}", frame->getLastErrorInfo().getErrorInfo().c_str())
		return false;
	}
	return true;
}

//! ��ʼ���߼�
bool Intermediary::InitLock() {


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
	parme->aiStatus = true;		//! ai����״̬

	while (!parme->uiSendStop) {
		//! ��ͼ
		dxgi->BitmapToMat(&img);
		//! ����
		frame->Detect(img);
		//! ����

		//! ����
		DrawBox(img);
	}

	//! ��λ�ź�
	parme->aiStatus = false;

	//! �رմ���
	if (cv::getWindowProperty(OPENCV_WINDOWS_NAME, cv::WND_PROP_VISIBLE))
		cv::destroyWindow(OPENCV_WINDOWS_NAME);
	return true;
}

//! ���ƿ�
void Intermediary::DrawBox(cv::Mat& img) {
	//! ���ƿ�
	if (parme->showWindows) {
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
}

//! ��ں���
#if 1 // test
int main() {
	std::cout << " test" << std::endl;
	start_thread(true);

	return 0;
}
#endif