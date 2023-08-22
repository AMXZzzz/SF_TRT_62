#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include "sf-trt.h"
#include "factory-base.h"




class YOLO {
public:

	YOLO(float* conf, float* iou, bool* showWindow, Process* process):_conf(conf),_iou(iou), _showWindow(showWindow) ,_process(process){}

	// --------------------------- 获取信息API --------------------------- // 
	//! 获取配置文件类型
	virtual sf::Type::YoloType getConfigType() {
		return sf::Type::YoloType::TYPE_UNKONE;
	}

	//! 获取置信度的指针
	//! 常用于交接或者验证
	virtual	float* getConfidencePtr() {
		return this->_conf;
	}

	//! 获取iou的指针
	//! 常用于交接
	virtual	float* getIOUPtr() {
		return this->_iou;
	}

	//！ 显示窗口
	virtual bool* getShowWindowPtr() {
		return this->_showWindow;
	}

	//! 获取归一化的值
	virtual	const float getNormalized() {
		return this->Normalized;
	}

	//! 获取输入层的shape
	virtual	std::vector<int64_t> getInputDims() {
		return this->input_dims;
	}

	//! 获取输入层的shape
	virtual	std::vector<int64_t> getOutputDims() {
		return this->output_dims;
	}

	//! 获取输入节点的名称
	//! 二级指针
	virtual	const char** getInputName() {
		return this->input_name;
	}

	//! 获取输出节点的名称
	//! 二级指针
	virtual	const char** getOutputName() {
		return this->output_name;
	}


	//! 获取输入图片的size
	virtual const cv::Size getImageSize() {
		return cv::Size(this->input_dims[2], this->input_dims[3]);
	}

	// --------------------------- 设置API --------------------------- // 
	//! 设置输入维度
	//! 给初始化用的
	virtual void setInputDims(std::vector<int64_t>& input) {
		input_dims.assign(input.begin(), input.end());
	}

	//! 设置输出维度
	virtual void setOutputDims(std::vector<int64_t>& output) {
		output_dims.assign(output.begin(), output.end());
	}

	//! 验证维度是否正确
	virtual bool dims_error(int64_t dim1, int64_t dim2) {
		return dim1 > dim2 ? false : true ;	// 默认是yolov5 和yolox的,yolov8需要重写
	}

	//! 验证配置表的完整性
	//! 检查成员指针是否有效
	//! 极为重要!!!
	virtual int verifyIntegrity() {
		if (!_conf) return 1;
		if (!_iou) return 1;
		if (!_process) return 1;
		return 0;
	}

	// 显示推理窗口
	void DrawBox(cv::Mat& img) {
		if (!*_showWindow) {
			if (cv::getWindowProperty(WINDOWS_NAME, cv::WND_PROP_VISIBLE)) {
				cv::destroyAllWindows();
			}
			return;
		}

		const auto& indices = _process->_indices;
		const auto& boxes = _process->_boxes;
		const int numIndices = indices.size();

		cv::Scalar greenColor(0, 255, 0);
		int lineThickness = 2;

		for (int i = 0; i < numIndices; ++i) {
			const auto& box = boxes[int(indices[i])];
			const float halfWidth = box.width * 0.5f;
			const float halfHeight = box.height * 0.5f;

			cv::Rect rect(box.x - halfWidth, box.y - halfHeight, box.width, box.height);
			cv::rectangle(img, rect, greenColor, lineThickness, cv::LINE_8, 0);
		}

		cv::imshow(WINDOWS_NAME, img);

		if (cv::getWindowProperty(WINDOWS_NAME, cv::WND_PROP_VISIBLE)) {

			cv::setWindowProperty(WINDOWS_NAME, cv::WND_PROP_TOPMOST, 1);
		}
		cv::waitKey(1);
	}










	//! 对输出进行解码
	virtual void DecodeOutput(float* output, cv::Mat& img) = 0;

	void Release() {
		delete this;
	}

	//! 释放父类指针指向的对象
	//! 若没有虚析构,则只会释放父类本身内存
	//! 出现无法释放派生类的内存
	virtual ~YOLO() {};
protected:

	//! DMld
	const char* input_name[1] = { "images" };
	const char* output_name[1] = { "output" };

	float* _conf = nullptr;
	float* _iou = nullptr;
	bool* _showWindow = nullptr;
	Process* _process = nullptr;

	float Normalized = 1.f / 255.f;
	std::vector<int64_t> input_dims = {};
	std::vector<int64_t> output_dims = {};
};

class YoloFactory :public IFactory {
public:
	static YoloFactory* createYoloFactory() {
		return new YoloFactory();
	}

	IStates QueryInterface(void** yolo) override;

	virtual IStates AcquireYoloType(sf::Type::YoloType type);

	//! 获取创建对象的来源属性 
	virtual IStates AcquireProcessPtr(Process* process);

	//! 获取置信度指针
	virtual IStates AcquireConfidencePtr(float* conf);

	//! 获取IOU指针
	virtual IStates AcquireIOUPtr(float* iou);

	//！窗口显示
	virtual IStates AcquireShowWindowPtr(bool* showWindow);

	//! 释放派生类和基类
	void Release() override;

	//! 虚析构 用于抽象工厂派生类
	virtual ~YoloFactory() {};

protected:
	YoloFactory() {}
	IStates checkMemberPtr();

	float* _conf = nullptr;
	float* _iou = nullptr;
	bool* _showWindow = false;
	struct Process* _process = nullptr;
	sf::Type::YoloType _type;
};


