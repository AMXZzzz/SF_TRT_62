#pragma once
#include <vector>
#include <windows.h>
#include <opencv2/opencv.hpp>

#ifndef API_EXPORT
#define SF_API extern "C" __declspec(dllimport)
#else
#define SF_API extern "C" __declspec(dllexport)
#endif

//! yolo配置类型的命名空间
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

//! 中间目标结构体，内部使用
struct Process {
	std::vector<cv::Rect> boxes;
	std::vector<int> indices;
	std::vector<int> classes;
	std::vector<float> confidences;
};

//! 需要填充该结构体
struct YOLOINFO {
	sf::Type::YoloType type;
	float* conf;
	float* iou;
	Process* process;
};

class YOLO {
public:

	YOLO(float* conf, float* iou, Process* process) :m_conf(conf), m_iou(iou), m_process(process) {}

	// --------------------------- 获取信息API --------------------------- // 
	//! 获取配置文件类型
	virtual sf::Type::YoloType getYoloType();

	//! 获取置信度的指针
	//! 常用于交接或者验证
	virtual	float* getConfidencePtr();

	//! 获取iou的指针
	//! 常用于交接
	virtual	float* getIOUPtr();

	//! 获取归一化的值
	virtual	const float getNormalized();

	//! 获取输入层的shape
	virtual	std::vector<int64_t> getInputDims();

	//! 获取输入层的shape
	virtual	std::vector<int64_t> getOutputDims();

	//! 获取输入节点的名称
	//! 二级指针
	virtual	const char** getInputName();

	//! 获取输出节点的名称
	//! 二级指针
	virtual	const char** getOutputName();

	//! 获取输入图片的size
	virtual const cv::Size getImageSize();

	// --------------------------- 设置API --------------------------- // 
	//! 设置输入维度
	//! 给初始化用的
	virtual void setInputDims(std::vector<int64_t>& input);

	//! 设置输出维度
	virtual void setOutputDims(std::vector<int64_t>& output);

	//! 验证维度是否正确
	virtual bool dims_error(int64_t dim1, int64_t dim2);

	//! 对输出进行解码
	virtual void DecodeOutput(float* output) = 0;

	//! 释放函数
	virtual void Release() = 0;

	//! 释放父类指针指向的对象
	//! 若没有虚析构,则只会释放父类本身内存
	//! 出现无法释放派生类的内存
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


//工厂函数，用于创建对象
namespace sf {
	SF_API YOLO* WINAPI createYoloTable(YOLOINFO* info);
}

