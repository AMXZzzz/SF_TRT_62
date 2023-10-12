#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include "yolo.h"
#include "IStates.h"

#ifndef API_EXPORT
#define SF_API extern "C" __declspec(dllimport)
#else
#define SF_API extern "C" __declspec(dllexport)
#endif

//! �������
namespace sf {
	namespace Type {
		//! ������
		enum FrameType :int {
			TRT_FRAME = 0,
			DML_FRAME = 1,
		};
	}
}

//! ��Ҫ���ýṹ��
struct FRAMEINFO{
	YOLO* yolo;								//! yolo���ñ�
	sf::Type::FrameType frame_type;			//! �������
	std::shared_ptr<spdlog::logger> logger;	//! ��־ָ��
	int equipment;							//! �����豸
};

//! ��ܻ���
class Frame {
public:
	//! ��ʼ�������Ҫ����yolo���ñ���־ָ�룬�����豸
	Frame(YOLO* yolo, const std::shared_ptr<spdlog::logger>& logger, int equipment)
		: m_yolo(yolo), m_logger(logger), m_equipment(equipment) {
	}

	//! ����ģ��
	virtual bool AnalyticalModel(const char* engine_path) = 0;

	//! ����ģ��
	virtual bool AnalyticalModel(const std::string engine_path) = 0;

	//! ����ӿ�
	virtual void Detect(cv::Mat&) = 0;

	//! �ͷŽӿ�
	virtual void Release() = 0;

	//! ��ȡ������Ϣ
	virtual	IStates getLastErrorInfo() = 0;

	//! ���������
	virtual ~Frame() {};

protected:
	YOLO* m_yolo;										//! yolo���ñ�
	std::shared_ptr<spdlog::logger> m_logger = nullptr;	//! ָ����־��ָ��
	int m_equipment = 0;								//! �����豸
};


namespace sf {
	SF_API Frame* CreateFrame(FRAMEINFO* info);
}