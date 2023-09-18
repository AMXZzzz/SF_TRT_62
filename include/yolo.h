#pragma once
#include <vector>
#include <windows.h>
#include <opencv2/opencv.hpp>

#ifndef API_EXPORT
#define SF_API extern "C" __declspec(dllimport)
#else
#define SF_API extern "C" __declspec(dllexport)
#endif

//! yolo�������͵������ռ�
namespace sf {
	namespace Type {
		enum YoloType :int {
			TYPE_UNKONE = 0,
			TYPE_YOLOV5 = 1,
			TYPE_YOLOV8 = 2,
			TYPE_YOLOX = 3,
		};
	}
}

//! �м�Ŀ��ṹ�壬�ڲ�ʹ��
struct Process {
	std::vector<cv::Rect> boxes;
	std::vector<int> indices;
	std::vector<int> classes;
	std::vector<float> confidences;
};

//! ��Ҫ���ýṹ��
struct YOLOINFO {
	sf::Type::YoloType type;
	float* conf;
	float* iou;
	Process* process;
};

class YOLO {
public:

	YOLO(float* conf, float* iou, Process* process) :m_conf(conf), m_iou(iou), m_process(process) {}

	// --------------------------- ��ȡ��ϢAPI --------------------------- // 
	//! ��ȡ�����ļ�����
	virtual sf::Type::YoloType getYoloType();

	//! ��ȡ���Ŷȵ�ָ��
	//! �����ڽ��ӻ�����֤
	virtual	float* getConfidencePtr();

	//! ��ȡiou��ָ��
	//! �����ڽ���
	virtual	float* getIOUPtr();

	//! ��ȡ��һ����ֵ
	virtual	const float getNormalized();

	//! ��ȡ������shape
	virtual	std::vector<int64_t> getInputDims();

	//! ��ȡ������shape
	virtual	std::vector<int64_t> getOutputDims();

	//! ��ȡ����ڵ������
	//! ����ָ��
	virtual	const char** getInputName();

	//! ��ȡ����ڵ������
	//! ����ָ��
	virtual	const char** getOutputName();

	//! ��ȡ����ͼƬ��size
	virtual const cv::Size getImageSize();

	// --------------------------- ����API --------------------------- // 
	//! ��������ά��
	//! ����ʼ���õ�
	virtual void setInputDims(std::vector<int64_t>& input);

	//! �������ά��
	virtual void setOutputDims(std::vector<int64_t>& output);

	//! ��֤ά���Ƿ���ȷ
	virtual bool dims_error(int64_t dim1, int64_t dim2);

	//! ��������н���
	virtual void DecodeOutput(float* output) = 0;

	//! �ͷź���
	virtual void Release() = 0;

	//! �ͷŸ���ָ��ָ��Ķ���
	//! ��û��������,��ֻ���ͷŸ��౾���ڴ�
	//! �����޷��ͷ���������ڴ�
	virtual ~YOLO() {};

protected:

	const char* input_name[1] = { "images" };
	const char* output_name[1] = { "output" };

	float* m_conf = nullptr;
	float* m_iou = nullptr;
	Process* m_process = nullptr;

	float normalized = 1.f / 255.f;
	std::vector<int64_t> m_input_dims = {};
	std::vector<int64_t> m_output_dims = {};
};


//�������������ڴ�������
namespace sf {
	SF_API YOLO* WINAPI createYoloTable(YOLOINFO* info);
}

