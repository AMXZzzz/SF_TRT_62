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
	//! 开启了类别筛选
	if (category_filter) {				
		if (new_indices->empty()) {		//! 筛选后，不存在符合条件的
			return -1;					//! 返回-1
		}
		//! 存在符合条件的，返回最小值的索引
		return (*new_indices)[std::distance(std::begin(*distance), std::min_element(std::begin(*distance), std::end(*distance)))];
	}

	//! 筛选后，不存在符合条件的
	if (indices->empty()) {
		return -1;
	}
	//! 存在符合条件的，返回最小值的索引
	return (*indices)[std::distance(std::begin(*distance), std::min_element(std::begin(*distance), std::end(*distance)))];
}

void Functional::categoryFilter(std::vector<float>* distance,
	std::vector<int>* indices,
	int idx
) {
	//! 从开新的索引表
	indices->push_back(m_process->indices[idx]);
	//! 获取该目标的欧式距离		
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
	if (m_sharedmemory->s_data.aim_position) {
		target.center_y = (target.origin_x + target.height) - (target.height * m_sharedmemory->s_data.aim_position);
	}

	//! 相对距离
	float x = (m_point.origin_x + target.center_x) - m_point.center_x;
	float y = (m_point.origin_y + target.center_y) - m_point.center_y;

	//! 范围跳过
	if (abs(x) > m_sharedmemory->s_data.aim_range * 0.5 || abs(y) > m_sharedmemory->s_data.aim_range * 0.5) return;

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
