#include "yolov8.h"

const char** YOLOV8::getOutputName() {
	return this->output_name;
}

sf::Type::YoloType YOLOV8::getYoloType() {
	return sf::Type::YoloType::TYPE_YOLOV8;
}
bool YOLOV8::dims_error(int64_t dim1, int64_t dim2) {
	return dim1 < dim2 ? false : true;	// 默认是yolov5 和yolox的,yolov8需要重写
}

void YOLOV8::DecodeOutput(float* output) {

	int class_num = m_output_dims[1] - 4;		// yolov8 is -4

	m_process->boxes.clear();
	m_process->classes.clear();
	m_process->confidences.clear();
	m_process->indices.clear();

	// yolov8的官方后处理是低性能
	cv::Mat outputs((4 + class_num), m_output_dims[2], CV_32F, output);
	outputs = outputs.t();	// 1 84 2520  -> 1 25200 84		// 导出onnx进行优化		
	for (int i = 0; i < m_output_dims[2]; i++) {
		auto rowPtr = outputs.row(i).ptr<float>();
		auto bboxesPtr = rowPtr;
		auto scoresPtr = rowPtr + 4;

		auto max_class_scores = std::max_element(scoresPtr, scoresPtr + class_num);

		if (*max_class_scores > *m_conf) {

			cv::Rect_<float> bbox;
			bbox.x = *(bboxesPtr + 0);
			bbox.y = *(bboxesPtr + 1);
			bbox.width = *(bboxesPtr + 2);
			bbox.height = *(bboxesPtr + 3);

			m_process->boxes.push_back(bbox);
			m_process->confidences.push_back(*max_class_scores);
			m_process->classes.push_back((max_class_scores - scoresPtr));
		}
	}
	cv::dnn::NMSBoxes(m_process->boxes, m_process->confidences, *m_conf, *m_iou, m_process->indices);
}
