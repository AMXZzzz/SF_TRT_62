#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include "sf-trt.h"
#include "factory-base.h"




class YOLO {
public:

	YOLO(float* conf, float* iou, bool* showWindow, Process* process):_conf(conf),_iou(iou), _showWindow(showWindow) ,_process(process){}

	// --------------------------- ��ȡ��ϢAPI --------------------------- // 
	//! ��ȡ�����ļ�����
	virtual sf::Type::YoloType getConfigType() {
		return sf::Type::YoloType::TYPE_UNKONE;
	}

	//! ��ȡ���Ŷȵ�ָ��
	//! �����ڽ��ӻ�����֤
	virtual	float* getConfidencePtr() {
		return this->_conf;
	}

	//! ��ȡiou��ָ��
	//! �����ڽ���
	virtual	float* getIOUPtr() {
		return this->_iou;
	}

	//�� ��ʾ����
	virtual bool* getShowWindowPtr() {
		return this->_showWindow;
	}

	//! ��ȡ��һ����ֵ
	virtual	const float getNormalized() {
		return this->Normalized;
	}

	//! ��ȡ������shape
	virtual	std::vector<int64_t> getInputDims() {
		return this->input_dims;
	}

	//! ��ȡ������shape
	virtual	std::vector<int64_t> getOutputDims() {
		return this->output_dims;
	}

	//! ��ȡ����ڵ������
	//! ����ָ��
	virtual	const char** getInputName() {
		return this->input_name;
	}

	//! ��ȡ����ڵ������
	//! ����ָ��
	virtual	const char** getOutputName() {
		return this->output_name;
	}


	//! ��ȡ����ͼƬ��size
	virtual const cv::Size getImageSize() {
		return cv::Size(this->input_dims[2], this->input_dims[3]);
	}

	// --------------------------- ����API --------------------------- // 
	//! ��������ά��
	//! ����ʼ���õ�
	virtual void setInputDims(std::vector<int64_t>& input) {
		input_dims.assign(input.begin(), input.end());
	}

	//! �������ά��
	virtual void setOutputDims(std::vector<int64_t>& output) {
		output_dims.assign(output.begin(), output.end());
	}

	//! ��֤ά���Ƿ���ȷ
	virtual bool dims_error(int64_t dim1, int64_t dim2) {
		return dim1 > dim2 ? false : true ;	// Ĭ����yolov5 ��yolox��,yolov8��Ҫ��д
	}

	//! ��֤���ñ��������
	//! ����Աָ���Ƿ���Ч
	//! ��Ϊ��Ҫ!!!
	virtual int verifyIntegrity() {
		if (!_conf) return 1;
		if (!_iou) return 1;
		if (!_process) return 1;
		return 0;
	}

	// ��ʾ������
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










	//! ��������н���
	virtual void DecodeOutput(float* output, cv::Mat& img) = 0;

	void Release() {
		delete this;
	}

	//! �ͷŸ���ָ��ָ��Ķ���
	//! ��û��������,��ֻ���ͷŸ��౾���ڴ�
	//! �����޷��ͷ���������ڴ�
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

	//! ��ȡ�����������Դ���� 
	virtual IStates AcquireProcessPtr(Process* process);

	//! ��ȡ���Ŷ�ָ��
	virtual IStates AcquireConfidencePtr(float* conf);

	//! ��ȡIOUָ��
	virtual IStates AcquireIOUPtr(float* iou);

	//��������ʾ
	virtual IStates AcquireShowWindowPtr(bool* showWindow);

	//! �ͷ�������ͻ���
	void Release() override;

	//! ������ ���ڳ��󹤳�������
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


