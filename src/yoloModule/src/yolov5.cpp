#include "yolov5.h"

sf::Type::YoloType YOLOV5::getYoloType() {
	return sf::Type::YoloType::TYPE_YOLOV5;
};

void YOLOV5::Release() {
	delete this;
}

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
		temp.x = ((float*)output)[index];
		temp.y = ((float*)output)[index + 1];
		temp.width = ((float*)output)[index + 2];
		temp.height = ((float*)output)[index + 3];
		m_process->boxes.push_back(temp);
		m_process->classes.push_back(classidx);
		m_process->confidences.push_back(confidence);
	}

	cv::dnn::NMSBoxes(m_process->boxes, m_process->confidences, *m_conf, *m_iou, m_process->indices);
}
