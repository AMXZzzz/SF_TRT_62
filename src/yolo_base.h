/*
* ����������
* ������ǿ�Դ��������������ʹ�á��޸ĺͷ�������ʹ�� GNU ͨ�ù������֤�汾 2��GPLv2�������
* ��ע�⣬�ڸ���GPLv2���������֮ǰ�������������������
* 1. �κ��޸ĺ�������Ʒ����ͬ������GPLv2����������������Ʒ���ṩGPLv2�������ı���
* 2. �����뱣��ԭʼ����İ�Ȩͨ�棬����������Ŀ����ȷָ��ԭʼ���ߺ��������Դ��
* 3. ������ַ���������������ṩ������Դ���룬������GPLv2��Ҫ������������޸ĺ�������Ʒ��Դ���롣
* 4. ������ǰ�"ԭ��"�ṩ�ģ��������κ���ʾ��ʾ�ı�֤�����߶���ʹ�ñ������ɵ��κ�ֱ�ӡ���ӡ�ż�������⡢���ͻ�ͽ����𺦲��е����Ρ�
* ʹ�ô˴��뼴��ʾ��ͬ������GPLv2������Ҫ�������������������
* ����ϸ�Ķ�GNUͨ�ù������֤�汾2���˽������ϸ��Ϣ���������� http ://www.gnu.org/licenses/gpl-2.0.html ���ҵ�GPLv2�������ı���
* [��������]: bilibili������Ϣ
* [��ַ]:https://github.com/AMXZzzz/SF_TRT_62.git
* [����]: 2023/10/26
*/
#pragma once
#include <vector>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include "base_type.h"

#define CENTER_COORDINATE  0	//! ʹ�����ĵ�Ϊ����


//! yolo���͵������ռ�
namespace sf {
	namespace Type {
		enum YoloType :int {
			TYPE_UNKONE = 0,	//! ������
			TYPE_YOLOV5 = 1,	//! yolov5 ���� yolov7
			TYPE_YOLOV8 = 2,
			TYPE_YOLOX = 3,
		};
	}
}

class YOLO {
public:

	YOLO(float* conf, float* iou, Process* process);

	// --------------------------- ��ȡ��ϢAPI --------------------------- // 
	//! ��ȡyolo��������
	virtual sf::Type::YoloType getYoloType();

	//! ��ȡ���Ŷȵ�ָ��
	virtual	float* getConfidencePtr();

	//! ��ȡiou��ָ��
	virtual	float* getIOUPtr();

	//! ��ȡ��һ����ֵ
	virtual	const float getNormalized();

	//! ��ȡ������ά��
	virtual	std::vector<int64_t> getInputDims();

	//! ��ȡ������ά��
	virtual	std::vector<int64_t> getOutputDims();

	//! ��ȡ����ڵ�����ƣ�����ָ��
	virtual	const char** getInputName();

	//! ��ȡ����ڵ�����ƣ�����ָ��
	virtual	const char** getOutputName();

	//! ��ȡ����ͼƬ��size
	virtual const cv::Size getImageSize();

	// --------------------------- ������ϢAPI --------------------------- // 
	//! ��ʼ����������ά��
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
	virtual ~YOLO();

protected:

	const char* input_name[1] = { "images" };
	const char* output_name[1] = { "output" };

	float* m_conf = nullptr;
	float* m_iou = nullptr;
	Process* m_process = nullptr;

	float normalized = 1.f / 255.f;
	std::vector<int64_t> m_input_dims = {};
	std::vector<int64_t> m_output_dims = {};

private:
	YOLO();
};


//�������������ڴ�������
//! ��Ҫ���ýṹ��
struct YOLOINFO {
	sf::Type::YoloType type;
	float* conf;
	float* iou;
	Process* process;
};

namespace sf {
	YOLO* createYoloObject(YOLOINFO* info);
}

