#define API_EXPORT

#include "yolox.h"
#include "yolov5.h"
#include "yolov8.h"


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
	return dim1 > dim2 ? false : true;	// 默认是yolov5 和yolox的,yolov8需要重写
}

SF_API YOLO* WINAPI sf::createYoloTable(YOLOINFO* info) {
	switch (info->type) {
	case sf::Type::TYPE_YOLOV5: return new YOLOV5(info->conf, info->iou, info->process);
	case sf::Type::TYPE_YOLOV8: return new YOLOV8(info->conf, info->iou, info->process);
	case sf::Type::TYPE_YOLOX: return new YOLOX(info->conf, info->iou, info->process);
	default: return nullptr;
	}
}
