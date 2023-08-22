#include "yolov5.h"

sf::Type::YoloType sf::Pattern::YOLOV5::getConfigType() {
	return sf::Type::YoloType::TYPE_YOLOV5;
};

void sf::Pattern::YOLOV5::DecodeOutput(float* output, cv::Mat& img) {

	int class_num = output_dims[2] - 5;

	_process->_boxes.clear();
	_process->_classes.clear();
	_process->_confidences.clear();
	_process->_indices.clear();

	for (size_t i = 0; i < output_dims[1]; ++i) {
		int index = i * (5 + class_num);
		float confidence = output[index + 4];
		if (confidence <= *_conf) continue;

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
		_process->_boxes.push_back(temp);
		_process->_classes.push_back(classidx);
		_process->_confidences.push_back(confidence);
	}

	cv::dnn::NMSBoxes(_process->_boxes, _process->_confidences, *_conf, *_iou, _process->_indices);
	DrawBox(img);
}