#include "yolox.h"

const float YOLOX::getNormalized() {
	return this->Normalized;
}

sf::Type::YoloType YOLOX::getYoloType() {
	return sf::Type::YoloType::TYPE_YOLOX;
};

void YOLOX::DecodeOutput(float* output) {

	m_process->boxes.clear();
	m_process->classes.clear();
	m_process->confidences.clear();
	grid_strides.clear();
	m_process->indices.clear();

	generate_grids_and_stride();

	for (int anchor_idx = 0; anchor_idx < grid_strides.size(); ++anchor_idx) {
		const int grid0 = grid_strides[anchor_idx].gh; // H
		const int grid1 = grid_strides[anchor_idx].gw; // W
		const int stride = grid_strides[anchor_idx].stride; // stride
		const int basic_pos = anchor_idx * m_output_dims[2];

		//boxs 计算
		float x_center = (output[basic_pos + 0] + grid0) * stride;
		float y_center = (output[basic_pos + 1] + grid1) * stride;
		float w = exp(output[basic_pos + 2]) * stride;
		float h = exp(output[basic_pos + 3]) * stride;
		float x0 = x_center - w * 0.5f;     //中心点转原点   除法比乘法慢
		float y0 = y_center - h * 0.5f;
		float box_objectness = output[basic_pos + 4];

		//num_class的置信度
		for (int class_idx = 0; class_idx < m_output_dims[2] - 5; class_idx++) {
			float box_cls_score = output[basic_pos + 5 + class_idx];
			float box_prob = box_objectness * box_cls_score;

			//置信度筛选
			if (box_prob > *m_conf) {
				cv::Rect rect;
				rect.x = x_center;
				rect.y = y_center;
				rect.width = w;
				rect.height = h;

				//所有目标信息
				m_process->classes.push_back(class_idx);
				m_process->confidences.push_back((float)box_prob);
				m_process->boxes.push_back(rect);
			}
		}
	}
	cv::dnn::NMSBoxes(m_process->boxes, m_process->confidences, *m_conf, *m_iou, m_process->indices);
}

void YOLOX::Release() {
	delete this;
}

void YOLOX::generate_grids_and_stride() {

	int stride = 8;
	for (size_t i = 0; i < 3; i++) {
		int num_grid_w = m_input_dims[2] / stride;
		int num_grid_h = m_input_dims[3] / stride;

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