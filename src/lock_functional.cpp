#include "lock_functional.h"


IStates Functional::initLock() {

	//! 创建mousec对象
	m_mouse = sf::createMouse(&m_mouse_info);
	if (m_mouse == nullptr) {
		std::cout << "[debug]: 创建Mouse对象失败" << std::endl;
		return IStates(false, "创建Mouse对象失败");
	}
	//! mousec初始化
	IStates hr = m_mouse->init();
	if (hr.is_error()) {
		std::cout << "[debug]: Mouse对象init失败" << std::endl;
		return hr;
	}

	//! 初始化其他
	return IStates();
}

void getDistance(cv::Rect& box, IPoint& m_point, std::vector<float>* distance) {
#if CENTER_COORDINATE		//! box使用中心点坐标
	//! 转为屏幕坐标  截图位置原点 + 目标图片中心点 = 图片坐标
	float screen_x = m_point.origin_x + box.x;
	float screen_y = m_point.origin_y + box.y;

#else	//! box使用原点坐标
	float center_x = box.x + (box.width * 0.5);		//! 将图box的x原点坐标转为中心点坐标
	float center_y = box.y + (box.height * 0.5);	//! 将图box的y原点坐标转为中心点坐标
#endif
	//! 转为屏幕坐标  截图位置原点 + 目标图片中心点 = 图片坐标
	float screen_x = m_point.origin_x + center_x;
	float screen_y = m_point.origin_y + center_y;

	//! 尝试复用相对距离，减少计算(对比赋值和计算的延迟和消耗)
	//! 目标相对准星的距离   目标屏幕坐标 - 屏幕中心点 = 相对距离（斜边）
	float distance_x = screen_x - m_point.center_x;
	float distance_y = screen_y - m_point.center_y;

	//! 获取所有目标的相对距离的绝对值，最小的值就是最近的目标
	distance->push_back(pow(distance_x, 2) + pow(distance_y, 2));	//! 三家函数，x,y的平方和为斜边的平方
}

int getDistanceIdx(std::vector<float>* distance,
	std::vector<int>* new_indices,
	std::vector<int>* indices,
	bool category_filter) {
	int idx = 0;
	if (category_filter) {				//! 开启了类别筛选
		if (new_indices->empty()) {		//! 筛选后，不存在符合条件的
			return -1;					//! 返回-1
		}
		//! 存在符合条件的，返回最小值的索引
		return (*new_indices)[std::distance(std::begin(*distance), std::min_element(std::begin(*distance), std::end(*distance)))];
	}
	//! 存在符合条件的，返回最小值的索引
	return (*indices)[std::distance(std::begin(*distance), std::min_element(std::begin(*distance), std::end(*distance)))];
}

void Functional::setContainer(std::vector<float>* distance,
	std::vector<int>* indices,
	int idx
) {
	//! 从开新的索引表
	indices->push_back(m_process->indices[idx]);
	//! 获取该目标的相对坐标		
	getDistance(m_process->boxes[idx], m_point, distance);
}

void Functional::action() {

	std::vector<float> distance;			//! 欧式距离
	std::vector<int> new_indices;			//! 新建索引表

	//! 移动逻辑
	for (size_t i = 0; i < m_process->indices.size(); ++i) {
		if (m_sharedmemory->s_signal.category_filter) {
			switch (m_process->getClassName(i)) {	//! 第i个索引
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

	//! 获取最近的目标
	int idx = getDistanceIdx(&distance, &new_indices, &(m_process->indices), m_sharedmemory->s_signal.category_filter);
	if (idx == -1) {return;}

	//! 获取最近目标的信息
	IRect target{};
	target.width = m_process->boxes[idx].width;
	target.height = m_process->boxes[idx].height;

#if CENTER_COORDINATE	//! 中心点
	target.center_x = m_process->boxes[idx].x;
	target.center_y = m_process->boxes[idx].y;
	target.origin_x = m_process->boxes[idx].x - (m_process->boxes[idx].width * 0.5);
	target.origin_y = m_process->boxes[idx].y - (m_process->boxes[idx].height * 0.5);
#else	//! 原点坐标
	target.origin_x = m_process->boxes[idx].x;
	target.origin_y = m_process->boxes[idx].y;

	target.center_x = m_process->boxes[idx].x + (m_process->boxes[idx].width * 0.5);
	target.center_y = m_process->boxes[idx].y + (m_process->boxes[idx].height * 0.5);

#endif

	//! 偏移位置

	//! 相对距离
	
	//! 范围跳过

	//! 控制计算(使用对象，一个函数式（低配机器），一个多线程（高配机器）)

	//! 进入or跳过扳机

	//! 若不进入扳机，则进行单移动

	m_mouse->move(1, 1);

	//! 笔记
	//! 范围判存在bug，放弃使用相对距离进行判断，使用排名中心做计算
}

void Functional::Release() {
	delete this;                //! 等价于调用析构
}
