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
#include "lock_functional.h"


IStates Functional::initLock() {

	//! ����mousec����
	m_mouse = sf::createMouse(&m_mouse_info);
	if (m_mouse == nullptr) {
		std::cout << "[debug]: ����Mouse����ʧ��" << std::endl;
		return IStates(false, "����Mouse����ʧ��");
	}
	//! mousec��ʼ��
	IStates hr = m_mouse->init();
	if (hr.is_error()) {
		std::cout << "[debug]: Mouse����initʧ��" << std::endl;
		return hr;
	}

	//! ��ʼ������
	return IStates();
}

void getDistance(cv::Rect& box, IPoint& m_point, std::vector<float>* distance) {
#if CENTER_COORDINATE		//! boxʹ�����ĵ�����
	//! תΪ��Ļ����  ��ͼλ��ԭ�� + Ŀ��ͼƬ���ĵ� = ͼƬ����
	float screen_x = m_point.origin_x + box.x;
	float screen_y = m_point.origin_y + box.y;

#else	//! boxʹ��ԭ������
	float center_x = box.x + (box.width * 0.5);		//! ��ͼbox��xԭ������תΪ���ĵ�����
	float center_y = box.y + (box.height * 0.5);	//! ��ͼbox��yԭ������תΪ���ĵ�����
#endif
	//! תΪ��Ļ����  ��ͼλ��ԭ�� + Ŀ��ͼƬ���ĵ� = ͼƬ����
	float screen_x = m_point.origin_x + center_x;
	float screen_y = m_point.origin_y + center_y;

	//! ���Ը�����Ծ��룬���ټ���(�Աȸ�ֵ�ͼ�����ӳٺ�����)
	//! Ŀ�����׼�ǵľ���   Ŀ����Ļ���� - ��Ļ���ĵ� = ��Ծ��루б�ߣ�
	float distance_x = screen_x - m_point.center_x;
	float distance_y = screen_y - m_point.center_y;

	//! ��ȡ����Ŀ�����Ծ���ľ���ֵ����С��ֵ���������Ŀ��
	distance->push_back(pow(distance_x, 2) + pow(distance_y, 2));	//! ���Һ�����x,y��ƽ����Ϊб�ߵ�ƽ��
}

int getDistanceIdx(std::vector<float>* distance,
	std::vector<int>* new_indices,
	std::vector<int>* indices,
	bool category_filter) {

	int idx = 0;
	//! ���������ɸѡ
	if (category_filter) {				
		if (new_indices->empty()) {		//! ɸѡ�󣬲����ڷ���������
			return -1;					//! ����-1
		}
		//! ���ڷ��������ģ�������Сֵ������
		return (*new_indices)[std::distance(std::begin(*distance), std::min_element(std::begin(*distance), std::end(*distance)))];
	}

	//! ɸѡ�󣬲����ڷ���������
	if (indices->empty()) {
		return -1;
	}
	//! ���ڷ��������ģ�������Сֵ������
	return (*indices)[std::distance(std::begin(*distance), std::min_element(std::begin(*distance), std::end(*distance)))];
}

void Functional::categoryFilter(std::vector<float>* distance,
	std::vector<int>* indices,
	int idx
) {
	//! �ӿ��µ�������
	indices->push_back(m_process->indices[idx]);
	//! ��ȡ��Ŀ���ŷʽ����		
	getDistance(m_process->boxes[idx], m_point, distance);
}

void Functional::action() {

	std::vector<float> distance;			//! ŷʽ����
	std::vector<int> new_indices;			//! �½�������

	//! �ƶ��߼�
	for (size_t i = 0; i < m_process->indices.size(); ++i) {
		if (m_sharedmemory->s_signal.category_filter) {
			switch (m_process->getClassName(i)) {	//! ��i������
			case 0:
				if (m_sharedmemory->s_signal.first_class) {
					categoryFilter(&distance, &new_indices, i);
				}
				break;
			case 1:
				if (m_sharedmemory->s_signal.second_class) {
					categoryFilter(&distance, &new_indices, i);
				}
				break;
			case 2:
				if (m_sharedmemory->s_signal.third_class) {
					categoryFilter(&distance, &new_indices, i);
				}
				break;
			case 3:
				if (m_sharedmemory->s_signal.fourth_class) {
					categoryFilter(&distance, &new_indices, i);
				}
				break;
			}
		}
		else {
			getDistance(m_process->boxes[i], m_point, &distance);
		}
	}

	//! ��ȡ�����Ŀ��
	int idx = getDistanceIdx(&distance, &new_indices, &(m_process->indices), m_sharedmemory->s_signal.category_filter);
	if (idx == -1) {return;}

	//! ��ȡ���Ŀ�����Ϣ
	IRect target{};
	target.width = m_process->boxes[idx].width;
	target.height = m_process->boxes[idx].height;

#if CENTER_COORDINATE	//! ���ĵ�
	target.center_x = m_process->boxes[idx].x;
	target.center_y = m_process->boxes[idx].y;
	target.origin_x = m_process->boxes[idx].x - (m_process->boxes[idx].width * 0.5);
	target.origin_y = m_process->boxes[idx].y - (m_process->boxes[idx].height * 0.5);
#else	//! ԭ������
	target.origin_x = m_process->boxes[idx].x;
	target.origin_y = m_process->boxes[idx].y;

	target.center_x = m_process->boxes[idx].x + (m_process->boxes[idx].width * 0.5);
	target.center_y = m_process->boxes[idx].y + (m_process->boxes[idx].height * 0.5);

#endif

	//! ƫ��λ��
	if (m_sharedmemory->s_data.aim_position) {
		target.center_y = (target.origin_x + target.height) - (target.height * m_sharedmemory->s_data.aim_position);
	}

	//! ��Ծ���
	float x = (m_point.origin_x + target.center_x) - m_point.center_x;
	float y = (m_point.origin_y + target.center_y) - m_point.center_y;

	//! ��Χ����
	if (abs(x) > m_sharedmemory->s_data.aim_range * 0.5 || abs(y) > m_sharedmemory->s_data.aim_range * 0.5) return;

	//! ���Ƽ���(ʹ�ö���һ������ʽ�������������һ�����̣߳����������)

	//! ����or�������

	//! ����������������е��ƶ�

	m_mouse->move(1, 1);

	//! �ʼ�
	//! ��Χ�д���bug������ʹ����Ծ�������жϣ�ʹ����������������
}

void Functional::Release() {
	delete this;                //! �ȼ��ڵ�������
}
