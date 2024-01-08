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

		//boxs ����
		float x_center = (output[basic_pos + 0] + grid0) * stride;
		float y_center = (output[basic_pos + 1] + grid1) * stride;
		float w = exp(output[basic_pos + 2]) * stride;
		float h = exp(output[basic_pos + 3]) * stride;
		float x0 = x_center - w * 0.5f;     //���ĵ�תԭ��   �����ȳ˷���
		float y0 = y_center - h * 0.5f;


		float box_objectness = output[basic_pos + 4];

		//num_class�����Ŷ�
		for (int class_idx = 0; class_idx < m_output_dims[2] - 5; class_idx++) {
			float box_cls_score = output[basic_pos + 5 + class_idx];
			float box_prob = box_objectness * box_cls_score;

			//���Ŷ�ɸѡ
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

				//����Ŀ����Ϣ
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
	std::cout << "[debug]: yolox ����" << std::endl;
}

YOLOX::~YOLOX() {
	std::cout << "[debug]: yolox ����" << std::endl;
}