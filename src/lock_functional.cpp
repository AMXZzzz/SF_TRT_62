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

typedef std::chrono::system_clock::time_point TimePoint;
static TimePoint auto_start = std::chrono::system_clock::now();
static TimePoint sleep_start = std::chrono::system_clock::now();

static bool coolDownTime(TimePoint start, int interval, int random) {
	//! ���
	int random_time = 0;
	if (random != 0) {
		random_time = (int)random * rand() / (RAND_MAX + 1);
	}
	//! ��CD
	return std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::system_clock::now() - start).count() >= (interval + random_time) * 1.0;
}

static inline bool inScopeX(TargetInfo* target,const float scale_x) {
	//! ���� x ����
	return ((target->width * 0.5) * scale_x) > abs(target->distance_x);
}

static inline bool inScopeY(TargetInfo* target,const float scale_up,const float scale_down) {
	//! ���׼����Ŀ�����ĵ������
	if (target->distance_y > 0) {
		//! ��׼λ�� - ����λ�� = �ϲ�ʵ�ʸ�(��Ķ���);  �ϲ�ʵ�ʸ� * �������� = �ϲ�������
		//! ��: Ŀ��ԭ��y������ = 230��Ŀ���ĸ߶�h = 100, ��Ŀ�����ĵ�y = 280�� ����Ϊ1.0
		//! ��: 280 - 230 = 1.0 = 50  
		float up = (target->center_y - target->origin_y) * scale_up;
		//! up �Ǹոռ�����ϲ������߶ȣ�Ϊ���� 50
		//! ��� �ϲ������߶� > ��Ծ��룬����׼�����ϲ����������趨�ı�����Χ��
		return (target->distance_y < up) ? true : false;		//! ֱ���˳����Ѿ�֪�����������Ҫ��������
	}

	//! ׼�ǲ����ϲ��ͼ����²���׼����Ŀ�����ĵ������
	if (target->distance_y < 0) {
		//! ��׼λ�� - �ײ�λ�� = �²�ʵ�ʸ�(��ĵײ�); �²�ʵ�ʸ� * �������� = �²�������  
		//! ��: Ŀ��ԭ��y������ = 230��Ŀ���ĸ߶�h = 100, ��Ŀ�����ĵ�y = 280�� ����Ϊ1.0
		//! ��: 280 - (230 + 100) * 1.0 = - 50		PS: �ײ�λ�� = ԭ��y + ���
		float down = (target->center_y - (target->origin_y + target->height)) * scale_down;
		//! ��ʱ down �Ǹ���,-50���أ�target->distance_y Ҳ�Ǹ�������-80����
		//! ��� �²�������(down) < target->distance_y��������׼�����²����������趨�ı�����Χ��,  PS: �����Ƚ� -50 > -80
		return  (down < target->distance_y) ? true : false;
	}

	//! �����ϲ���Ҳ�����²�����׼������Ŀ�����ĵ�  target->distance_y == 0
	return true;
}

static void getDistance(cv::Rect& box, IPoint* m_point, std::vector<float>* distance) {
#if CENTER_COORDINATE		//! boxʹ�����ĵ�����
	//! תΪ��Ļ����  ��ͼλ��ԭ�� + Ŀ��ͼƬ���ĵ� = ͼƬ����
	float screen_x = m_point.origin_x + box.x;
	float screen_y = m_point.origin_y + box.y;

#else	//! boxʹ��ԭ������
	float center_x = box.x + (box.width * 0.5);		//! ��ͼbox��xԭ������תΪ���ĵ�����
	float center_y = box.y + (box.height * 0.5);	//! ��ͼbox��yԭ������תΪ���ĵ�����
#endif
	//! תΪ��Ļ����  ��ͼλ��ԭ�� + Ŀ��ͼƬ���ĵ� = ͼƬ����
	float screen_x = m_point->origin_x + center_x;
	float screen_y = m_point->origin_y + center_y;

	//! ���Ը�����Ծ��룬���ټ���(�Աȸ�ֵ�ͼ�����ӳٺ�����)
	//! Ŀ�����׼�ǵľ���   Ŀ����Ļ���� - ��Ļ���ĵ� = ��Ծ��루б�ߣ�
	float distance_x = screen_x - m_point->center_x;
	float distance_y = screen_y - m_point->center_y;

	//! ��ȡ����Ŀ�����Ծ���ľ���ֵ����С��ֵ���������Ŀ��
	distance->push_back(pow(distance_x, 2) + pow(distance_y, 2));	//! ���Һ�����x,y��ƽ����Ϊб�ߵ�ƽ��
}

static int getDistanceIdx(std::vector<float>* distance,
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

float MaxMovePixel(const float input, const int max) {
	if (abs(input) > max) {
		if (input > 0)
			return (max);
		if (input < 0)			// ���ܰ���0�������ƶ�ֵ����
			return (-max);
	}
	//! δ���з���ԭֵ
	return input;
}


void Functional::categoryFilter(std::vector<float>* distance,
	std::vector<int>* indices,
	int idx) {
	//! �ӿ��µ�������
	indices->push_back(m_process->indices[idx]);
	//! ��ȡ��Ŀ���ŷʽ����		
	getDistance(m_process->boxes[idx], m_point, distance);
}

void Functional::executeTrigger(TargetInfo* target) {
	//! ִ�а��
	if (inScopeX(target, m_sharedmemory->s_data.auto_trigger_x_scale) &&
		inScopeY(target, m_sharedmemory->s_data.auto_trigger_up_scale, m_sharedmemory->s_data.auto_trigger_down_scale) &&
		coolDownTime(auto_start, m_sharedmemory->s_data.delay_base, m_sharedmemory->s_data.delay_delay)) {	//! ��ȴ
		//! �����������
		if (m_mouse->monitor(m_sharedmemory->s_data.auto_key)) {
			m_mouse->trigger();
		}
		auto_start = std::chrono::system_clock::now();  	//! ������һ��CD
	}
}

void Functional::autoTrigger(TargetInfo* target) {

	//! �����ʽ
	switch (m_sharedmemory->s_data.auto_model) {
#pragma region �ƶ����
	case 0:
		if (m_mouse->monitor(m_sharedmemory->s_data.aim_key)) {
			//! �ƶ�
			m_mouse->move(static_cast<int>(target->move_x), static_cast<int>(target->move_y));
			//! ����ڷ�Χ�ڣ�ִ�а��
			executeTrigger(target);
		}
		break;
#pragma endregion comment
#pragma region �ܵ���
	case 1:
		//! ����ڷ�Χ�ڣ�ִ�а��
		executeTrigger(target);
		break;
#pragma endregion comment
#pragma region �ӳٰ��
	case 2:
		if (m_mouse->monitor(m_sharedmemory->s_data.aim_key)) {
			m_mouse->move(static_cast<int>(target->move_x), static_cast<int>(target->move_y));
			//! ��CD��
			if (coolDownTime(sleep_start, m_sharedmemory->s_data.delay_base, m_sharedmemory->s_data.delay_delay)) {
				//! ����ڷ�Χ�ڣ�ִ�а��
				if (inScopeX(target, m_sharedmemory->s_data.auto_trigger_x_scale) &&
					inScopeY(target, m_sharedmemory->s_data.auto_trigger_up_scale, m_sharedmemory->s_data.auto_trigger_down_scale)) {	//! ��ȴ
					if (m_mouse->monitor(m_sharedmemory->s_data.auto_key)) {
						m_mouse->trigger();
					}
					sleep_start = std::chrono::system_clock::now();		//! ������һ���ӳ�CD
				}
			}
		}
		break;
#pragma endregion comment
	}
}

void Functional::onlyMcove(TargetInfo* target) {
	if (m_sharedmemory->s_signal.keep_move == true) {
		m_mouse->move(static_cast<int>(target->move_x), static_cast<int>(target->move_y));
	}
	else {
		if (m_mouse->monitor(m_sharedmemory->s_data.aim_key)) {
			m_mouse->move(static_cast<int>(target->move_x), static_cast<int>(target->move_y));
		}
	}
}

void Functional::action() {
	//! ����ʱ���
	if (!m_mouse->monitor(m_sharedmemory->s_data.aim_key)) {
		sleep_start = std::chrono::system_clock::now();
	}

	//! ������
	if (m_process->indices.size() == 0) {
		return;
	}

	std::vector<float> distance;			//! ŷʽ����
	std::vector<int> new_indices;			//! ��������

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
	if (idx == -1) {return;}		//! -1 = û�з��ϵ�Ŀ��
	
	//! ��ȡ���Ŀ�����Ϣ
	TargetInfo target{};
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
		target.center_y = (target.origin_y + target.height) - (target.height * m_sharedmemory->s_data.aim_position);
	}

	//! ��Ծ���
	target.distance_x = (m_point->origin_x + target.center_x) - m_point->center_x;
	target.distance_y = (m_point->origin_y + target.center_y) - m_point->center_y;

	//! ��Χ����
	if (abs(target.distance_x) > m_sharedmemory->s_data.aim_range * 0.5 || abs(target.distance_y) > m_sharedmemory->s_data.aim_range * 0.5) return;

	//! ���Ƽ���(ʹ�ö��󣬺���ʽ����������������̣߳����������)
	target.move_x = control->control_x(target.distance_x, m_sharedmemory->s_data);
	target.move_y = control->control_y(target.distance_y, m_sharedmemory->s_data);

	//! ����ƶ�
	target.move_x = MaxMovePixel(target.move_x, m_sharedmemory->s_data.max_pixel);
	target.move_y = MaxMovePixel(target.move_y, m_sharedmemory->s_data.max_pixel);

	//! ����ƶ� or ���ƶ�
	(m_sharedmemory->s_signal.auto_trigger == true) ? autoTrigger(&target) : onlyMcove(&target);

}

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

	//! ��ʼ������ control
	control = sf::createControlObj(m_control_manner);
	if (m_mouse == nullptr) {
		std::cout << "[debug]: ����control����ʧ��" << std::endl;
		return IStates(false, "����control����ʧ��");
	}

	//! control ��ʼ��
	hr = control->init();
	if (hr.is_error()) {
		std::cout << "[debug]: control����initʧ��" << std::endl;
		return hr;
	}
	return IStates();
}

void Functional::Release() {
	m_mouse->close();
	control->Release();
	delete this;                //! �ȼ��ڵ�������
}

Functional::Functional(LockInfo info) :LOCK(info) {
	std::cout << "[debug]: Functional ����" << std::endl;
}

Functional::~Functional() {
	std::cout << "[debug]: Functional ����" << std::endl; 
}
