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
#include "yolov5.h"

sf::Type::YoloType YOLOV5::getYoloType() {
	return sf::Type::YoloType::TYPE_YOLOV5;
};

void YOLOV5::DecodeOutput(float* output) {

	int class_num = m_output_dims[2] - 5;

	m_process->boxes.clear();
	m_process->classes.clear();
	m_process->confidences.clear();
	m_process->indices.clear();

	for (size_t i = 0; i < m_output_dims[1]; ++i) {
		int index = i * (5 + class_num);
		float confidence = output[index + 4];
		if (confidence <= *m_conf) continue;

		float max_class_scores = 0;
		int classidx = 0;
		for (size_t i = 0; i < class_num; ++i) {
			if (max_class_scores < output[index + (5 + i)]) {
				max_class_scores = output[index + (5 + i)];
				classidx = i;
			}
		}

		cv::Rect temp;
#if CENTER_COORDINATE
		temp.x = ((float*)output)[index];
		temp.y = ((float*)output)[index + 1];
		temp.width = ((float*)output)[index + 2];
		temp.height = ((float*)output)[index + 3];
#else
		temp.x = ((float*)output)[index] - (((float*)output)[index + 2] * 0.5);
		temp.y = ((float*)output)[index + 1] - (((float*)output)[index + 3] * 0.5);
		temp.width = ((float*)output)[index + 2];
		temp.height = ((float*)output)[index + 3];
#endif

		m_process->boxes.push_back(temp);
		m_process->classes.push_back(classidx);
		m_process->confidences.push_back(confidence);
	}

	cv::dnn::NMSBoxes(m_process->boxes, m_process->confidences, *m_conf, *m_iou, m_process->indices);
}

void YOLOV5::Release() {
	delete this;
}

YOLOV5::YOLOV5(float* conf, float* iou, Process* process) : YOLO(conf, iou, process) {
	std::cout << "[debug]: yolov5 ����" << std::endl;
}

YOLOV5::~YOLOV5() {
	std::cout << "[debug]: yolov5 ����" << std::endl;
}
