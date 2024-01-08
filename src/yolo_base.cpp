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
#include "yolo_base.h"
#include "yolov5.h"
#include "yolov8.h"
#include "yolox.h"

YOLO* sf::createYoloObject(YOLOINFO* info) {
	switch (info->type) {
	case sf::Type::TYPE_YOLOV5: return new YOLOV5(info->conf, info->iou, info->process);
	case sf::Type::TYPE_YOLOV8: return new YOLOV8(info->conf, info->iou, info->process);
	case sf::Type::TYPE_YOLOX: return new YOLOX(info->conf, info->iou, info->process);
	}
	return NULL;
}

sf::Type::YoloType YOLO::getYoloType() {
	return sf::Type::YoloType::TYPE_UNKONE;
}

float* YOLO::getConfidencePtr() {
	return this->m_conf;
}

float* YOLO::getIOUPtr() {
	return this->m_iou;
}

const float YOLO::getNormalized() {
	return this->normalized;
}

std::vector<int64_t> YOLO::getInputDims() {
	return this->m_input_dims;
}

std::vector<int64_t> YOLO::getOutputDims() {
	return this->m_output_dims;
}

const char** YOLO::getInputName() {
	return this->input_name;
}

const char** YOLO::getOutputName() {
	return this->output_name;
}

const cv::Size YOLO::getImageSize() {
	return cv::Size(this->m_input_dims[2], this->m_input_dims[3]);
}

void YOLO::setInputDims(std::vector<int64_t>& input) {
	m_input_dims.assign(input.begin(), input.end());
}

void YOLO::setOutputDims(std::vector<int64_t>& output) {
	m_output_dims.assign(output.begin(), output.end());
}

bool YOLO::dims_error(int64_t dim1, int64_t dim2) {
	return dim1 > dim2 ? false : true;	// Ĭ����yolov5 ��yolox��ά����֤, yolov8��Ҫ��д
}

YOLO::YOLO(float* conf, float* iou, Process* process) :m_conf(conf), m_iou(iou), m_process(process) {
	std::cout << "[debug]: yolo ���๹��" << std::endl;
}

YOLO::~YOLO() {
	std::cout << "[debug]: yolo ��������" << std::endl;
}

