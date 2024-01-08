/*
* 免责声明：
* 本软件是开源程序，您可以自由使用、修改和发布它，使用 GNU 通用公共许可证版本 2（GPLv2）的条款。
* 请注意，在根据GPLv2发布本软件之前，您必须遵守以下条款：
* 1. 任何修改和派生作品必须同样采用GPLv2，并在您的派生作品中提供GPLv2的完整文本。
* 2. 您必须保留原始代码的版权通告，并在您的项目中明确指明原始作者和软件的来源。
* 3. 如果您分发本软件，您必须提供完整的源代码，以满足GPLv2的要求。这包括您的修改和派生作品的源代码。
* 4. 本软件是按"原样"提供的，不附带任何明示或暗示的保证。作者对于使用本软件造成的任何直接、间接、偶发、特殊、典型或惩戒性损害不承担责任。
* 使用此代码即表示您同意遵守GPLv2的所有要求和免责声明的条件。
* 请仔细阅读GNU通用公共许可证版本2以了解更多详细信息。您可以在 http ://www.gnu.org/licenses/gpl-2.0.html 上找到GPLv2的完整文本。
* [作者姓名]: bilibili：随风而息
* [地址]:https://github.com/AMXZzzz/SF_TRT_62.git
* [日期]: 2023/10/26
*/
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
#if CENTER_COORDINATE
				rect.x = x_center;
				rect.y = y_center;
#else
				rect.x = x0;
				rect.y = y0;
#endif
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

void YOLOX::Release() {
	delete this;
}

YOLOX::YOLOX(float* conf, float* iou, Process* process) : YOLO(conf, iou, process) {
	std::cout << "[debug]: yolox 构造" << std::endl;
}

YOLOX::~YOLOX() {
	std::cout << "[debug]: yolox 析构" << std::endl;
}