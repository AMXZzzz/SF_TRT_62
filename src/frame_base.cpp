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
#include "frame_base.h"
#include "frame_dml.h"
#include "frame_trt.h"


Frame* sf::createFrameObject(FRAMEINFO* info) {
	switch (info->frame_type) {
	case sf::Type::FrameType::DML_FRAME: return new IDML(info->yolo, info->logger, info->equipment);
	case sf::Type::FrameType::TRT_FRAME: return new ITensorRt(info->yolo, info->logger, info->equipment);
	default: return nullptr;
	}
}

Frame::Frame(YOLO* yolo, const std::shared_ptr<spdlog::logger>& logger, int equipment) : m_yolo(yolo), m_logger(logger), m_equipment(equipment) {
	std::cout << "[debug]: Frame ���๹��" << std::endl;
}

Frame::~Frame() {
	std::cout << "[debug]: Frame ��������" << std::endl;
};
