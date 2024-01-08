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
#include "yolo_base.h"
#include "yolov5.h"
#include "yolov8.h"
#include "yolox.h"

YOLO* sf::createYoloObject(YOLOINFO* info) {
	switch (info->type) {
	case sf::Type::TYPE_YOLOV5: return new YOLOV5(info->conf, info->iou, info->process);
	case sf::Type::TYPE_YOLOV8: return new YOLOV8(info->conf, info->iou, info->process);
	case sf::Type::TYPE_YOLOX: return new YOLOX(info->conf, info->iou, info->process);
	}
	return NULL;
}

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
	return dim1 > dim2 ? false : true;	// 默认是yolov5 和yolox的维度验证, yolov8需要重写
}

YOLO::YOLO(float* conf, float* iou, Process* process) :m_conf(conf), m_iou(iou), m_process(process) {
	std::cout << "[debug]: yolo 基类构造" << std::endl;
}

YOLO::~YOLO() {
	std::cout << "[debug]: yolo 基类析构" << std::endl;
}

