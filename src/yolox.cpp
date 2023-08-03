#include "yolox.h"

const float sf::Pattern::YOLOX::getNormalized() {
	return this->Normalized;
}


sf::Type::YoloType sf::Pattern::YOLOX::getConfigType() {
	return sf::Type::YoloType::TYPE_YOLOX;
};

void sf::Pattern::YOLOX::DecodeOutput(float* output, cv::Mat& img) {

	_process->_boxes.clear();
	_process->_classes.clear();
	_process->_confidences.clear();
	grid_strides.clear();
	_process->_indices.clear();

	generate_grids_and_stride();

	for (int anchor_idx = 0; anchor_idx < grid_strides.size(); ++anchor_idx) {
		const int grid0 = grid_strides[anchor_idx].gh; // H
		const int grid1 = grid_strides[anchor_idx].gw; // W
		const int stride = grid_strides[anchor_idx].stride; // stride
		const int basic_pos = anchor_idx * output_dims[2];

		//boxs ����
		float x_center = (output[basic_pos + 0] + grid0) * stride;
		float y_center = (output[basic_pos + 1] + grid1) * stride;
		float w = exp(output[basic_pos + 2]) * stride;
		float h = exp(output[basic_pos + 3]) * stride;
		float x0 = x_center - w * 0.5f;     //���ĵ�תԭ��   �����ȳ˷���
		float y0 = y_center - h * 0.5f;
		float box_objectness = output[basic_pos + 4];

		//num_class�����Ŷ�
		for (int class_idx = 0; class_idx < output_dims[2] - 5; class_idx++) {
			float box_cls_score = output[basic_pos + 5 + class_idx];
			float box_prob = box_objectness * box_cls_score;

			//���Ŷ�ɸѡ
			if (box_prob > *_conf) {
				cv::Rect rect;
				rect.x = x_center;
				rect.y = y_center;
				rect.width = w;
				rect.height = h;

				//����Ŀ����Ϣ
				_process->_classes.push_back(class_idx);
				_process->_confidences.push_back((float)box_prob);
				_process->_boxes.push_back(rect);
			}
		}
	}
	cv::dnn::NMSBoxes(_process->_boxes, _process->_confidences, *_conf, *_iou, _process->_indices);
	DrawBox(img);
}

void sf::Pattern::YOLOX::generate_grids_and_stride() {

	int stride = 8;
	for (size_t i = 0; i < 3; i++) {
		int num_grid_w = input_dims[2] / stride;
		int num_grid_h = input_dims[3] / stride;

		for (int g1 = 0; g1 < num_grid_w; g1++) {
			for (int g0 = 0; g0 < num_grid_h; g0++) {
				GridAndStride gs{};
				gs.gh = g0;
				gs.gw = g1;
				gs.stride = stride;
				grid_strides.push_back(gs);
			}
		}
		stride *= 2;
	}
}