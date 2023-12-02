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
	if (category_filter) {				//! ���������ɸѡ
		if (new_indices->empty()) {		//! ɸѡ�󣬲����ڷ���������
			return -1;					//! ����-1
		}
		//! ���ڷ��������ģ�������Сֵ������
		return (*new_indices)[std::distance(std::begin(*distance), std::min_element(std::begin(*distance), std::end(*distance)))];
	}
	//! ���ڷ��������ģ�������Сֵ������
	return (*indices)[std::distance(std::begin(*distance), std::min_element(std::begin(*distance), std::end(*distance)))];
}

void Functional::setContainer(std::vector<float>* distance,
	std::vector<int>* indices,
	int idx
) {
	//! �ӿ��µ�������
	indices->push_back(m_process->indices[idx]);
	//! ��ȡ��Ŀ����������		
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
					setContainer(&distance, &new_indices, i);
				}
				break;
			case 1:
				if (m_sharedmemory->s_signal.second_class) {
					setContainer(&distance, &new_indices, i);
				}
				break;
			case 2:
				if (m_sharedmemory->s_signal.third_class) {
					setContainer(&distance, &new_indices, i);
				}
				break;
			case 3:
				if (m_sharedmemory->s_signal.fourth_class) {
					setContainer(&distance, &new_indices, i);
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

	//! ��Ծ���
	
	//! ��Χ����

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
