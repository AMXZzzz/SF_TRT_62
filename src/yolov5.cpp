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
#include "yolov5.h"

sf::Type::YoloType YOLOV5::getYoloType() {
	return sf::Type::YoloType::TYPE_YOLOV5;
};

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


void YOLOV5::Release() {
	std::cout << "yolov5释放" << std::endl;
	delete this;
}
