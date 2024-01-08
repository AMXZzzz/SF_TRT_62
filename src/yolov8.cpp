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
#include "yolov8.h"

const char** YOLOV8::getOutputName() {
	return this->output_name;
}

sf::Type::YoloType YOLOV8::getYoloType() {
	return sf::Type::YoloType::TYPE_YOLOV8;
}

bool YOLOV8::dims_error(int64_t dim1, int64_t dim2) {
	return dim1 < dim2 ? false : true;	// Ĭ����yolov5 ��yolox��,yolov8��Ҫ��д
}

void YOLOV8::DecodeOutput(float* output) {

	int class_num = m_output_dims[1] - 4;		// yolov8 is -4

	m_process->boxes.clear();
	m_process->classes.clear();
	m_process->confidences.clear();
	m_process->indices.clear();

	// yolov8�Ĺٷ������ǵ�����
	cv::Mat outputs((4 + class_num), m_output_dims[2], CV_32F, output);
	outputs = outputs.t();	// 1 84 2520  -> 1 25200 84		//�� ���Ե���onnx�����Ż�		
	for (int i = 0; i < m_output_dims[2]; i++) {
		auto rowPtr = outputs.row(i).ptr<float>();
		auto bboxesPtr = rowPtr;
		auto scoresPtr = rowPtr + 4;

		auto max_class_scores = std::max_element(scoresPtr, scoresPtr + class_num);

		if (*max_class_scores > *m_conf) {

			cv::Rect_<float> bbox;
#if CENTER_COORDINATE
			bbox.x = *(bboxesPtr + 0);
			bbox.y = *(bboxesPtr + 1);
			bbox.width = *(bboxesPtr + 2);
			bbox.height = *(bboxesPtr + 3);
#else
			bbox.width = *(bboxesPtr + 2);
			bbox.height = *(bboxesPtr + 3);
			bbox.x = *(bboxesPtr + 0) - (bbox.width * 0.5);
			bbox.y = *(bboxesPtr + 1) - (bbox.height * 0.5);
#endif
			m_process->boxes.push_back(bbox);
			m_process->confidences.push_back(*max_class_scores);
			m_process->classes.push_back((max_class_scores - scoresPtr));
		}
	}
	cv::dnn::NMSBoxes(m_process->boxes, m_process->confidences, *m_conf, *m_iou, m_process->indices);
}

void YOLOV8::Release() {
	std::cout << "yolov8 ����" << std::endl;
	delete this;
}

YOLOV8::YOLOV8(float* conf, float* iou, Process* process) :YOLO(conf, iou, process) {
	std::cout << "[debug]: yolov8 ����" << std::endl;
}

YOLOV8::~YOLOV8() {
	std::cout << "[debug]: yolov8 ����" << std::endl;
}


