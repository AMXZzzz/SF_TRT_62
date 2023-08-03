#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include "sf-trt.h"
#include "factory-base.h"

class YOLO {
public:

	YOLO(float* conf, float* iou, Process* process):_conf(conf),_iou(iou),_process(process){}

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

	// ���Թ���
	virtual void DrawBox(cv::Mat& img) {
		for (size_t i = 0; i < _process->_indices.size(); ++i) {
			rectangle(img, cv::Rect(_process->_boxes[int(_process->_indices[i])].x - (_process->_boxes[int(_process->_indices[i])].width * 0.5f),
				_process->_boxes[int(_process->_indices[i])].y - (_process->_boxes[int(_process->_indices[i])].height * 0.5f),
				_process->_boxes[int(_process->_indices[i])].width, _process->_boxes[int(_process->_indices[i])].height),
				cv::Scalar(0, 255, 0), 2, 8, 0);
		}
		cv::imshow(WINDOWS_NAME, img);
		cv::waitKey(1);
	};

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

	//! �ͷ�������ͻ���
	void Release() override;

	//! ������ ���ڳ��󹤳�������
	virtual ~YoloFactory() {};

protected:
	YoloFactory() {}
	IStates checkMemberPtr();

	float* _conf = nullptr;
	float* _iou = nullptr;
	struct Process* _process = nullptr;
	sf::Type::YoloType _type;
};


