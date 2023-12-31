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
	outputs = outputs.t();	// 1 84 2520  -> 1 25200 84		//！ 尝试导出onnx进行优化		
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
	std::cout << "yolov8 析构" << std::endl;
	delete this;
}

YOLOV8::YOLOV8(float* conf, float* iou, Process* process) :YOLO(conf, iou, process) {
	std::cout << "[debug]: yolov8 构造" << std::endl;
}

YOLOV8::~YOLOV8() {
	std::cout << "[debug]: yolov8 构造" << std::endl;
}


