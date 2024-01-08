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

typedef std::chrono::system_clock::time_point TimePoint;
static TimePoint auto_start = std::chrono::system_clock::now();
static TimePoint sleep_start = std::chrono::system_clock::now();

static bool coolDownTime(TimePoint start, int interval, int random) {
	//! 随机
	int random_time = 0;
	if (random != 0) {
		random_time = (int)random * rand() / (RAND_MAX + 1);
	}
	//! 在CD
	return std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::system_clock::now() - start).count() >= (interval + random_time) * 1.0;
}

static inline bool inScopeX(TargetInfo* target,const float scale_x) {
	//! 计算 x 比例
	return ((target->width * 0.5) * scale_x) > abs(target->distance_x);
}

static inline bool inScopeY(TargetInfo* target,const float scale_up,const float scale_down) {
	//! 如果准星在目标中心点的上面
	if (target->distance_y > 0) {
		//! 瞄准位置 - 顶层位置 = 上部实际高(框的顶部);  上部实际高 * 触发比例 = 上部触发高
		//! 例: 目标原点y的坐标 = 230，目标框的高度h = 100, 则目标中心点y = 280， 比例为1.0
		//! 解: 280 - 230 = 1.0 = 50  
		float up = (target->center_y - target->origin_y) * scale_up;
		//! up 是刚刚计算的上部触发高度，为正数 50
		//! 如果 上部触发高度 > 相对距离，表明准星在上部框内且在设定的比例范围内
		return (target->distance_y < up) ? true : false;		//! 直接退出，已经知道结果，不需要计算后面的
	}

	//! 准星不在上部就计算下部，准星在目标中心点的下面
	if (target->distance_y < 0) {
		//! 瞄准位置 - 底部位置 = 下部实际高(框的底部); 下部实际高 * 触发比例 = 下部触发高  
		//! 例: 目标原点y的坐标 = 230，目标框的高度h = 100, 则目标中心点y = 280， 比例为1.0
		//! 解: 280 - (230 + 100) * 1.0 = - 50		PS: 底部位置 = 原点y + 框高
		float down = (target->center_y - (target->origin_y + target->height)) * scale_down;
		//! 此时 down 是负数,-50像素，target->distance_y 也是负数，如-80像素
		//! 如果 下部触发高(down) < target->distance_y，表明在准星在下部框内且在设定的比例范围内,  PS: 负数比较 -50 > -80
		return  (down < target->distance_y) ? true : false;
	}

	//! 不在上部，也不再下部，即准星正中目标中心点  target->distance_y == 0
	return true;
}

static void getDistance(cv::Rect& box, IPoint* m_point, std::vector<float>* distance) {
#if CENTER_COORDINATE		//! box使用中心点坐标
	//! 转为屏幕坐标  截图位置原点 + 目标图片中心点 = 图片坐标
	float screen_x = m_point.origin_x + box.x;
	float screen_y = m_point.origin_y + box.y;

#else	//! box使用原点坐标
	float center_x = box.x + (box.width * 0.5);		//! 将图box的x原点坐标转为中心点坐标
	float center_y = box.y + (box.height * 0.5);	//! 将图box的y原点坐标转为中心点坐标
#endif
	//! 转为屏幕坐标  截图位置原点 + 目标图片中心点 = 图片坐标
	float screen_x = m_point->origin_x + center_x;
	float screen_y = m_point->origin_y + center_y;

	//! 尝试复用相对距离，减少计算(对比赋值和计算的延迟和消耗)
	//! 目标相对准星的距离   目标屏幕坐标 - 屏幕中心点 = 相对距离（斜边）
	float distance_x = screen_x - m_point->center_x;
	float distance_y = screen_y - m_point->center_y;

	//! 获取所有目标的相对距离的绝对值，最小的值就是最近的目标
	distance->push_back(pow(distance_x, 2) + pow(distance_y, 2));	//! 三家函数，x,y的平方和为斜边的平方
}

static int getDistanceIdx(std::vector<float>* distance,
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

float MaxMovePixel(const float input, const int max) {
	if (abs(input) > max) {
		if (input > 0)
			return (max);
		if (input < 0)			// 不能包含0，否者移动值错误
			return (-max);
	}
	//! 未命中返回原值
	return input;
}


void Functional::categoryFilter(std::vector<float>* distance,
	std::vector<int>* indices,
	int idx) {
	//! 从开新的索引表
	indices->push_back(m_process->indices[idx]);
	//! 获取该目标的欧式距离		
	getDistance(m_process->boxes[idx], m_point, distance);
}

void Functional::executeTrigger(TargetInfo* target) {
	//! 执行扳机
	if (inScopeX(target, m_sharedmemory->s_data.auto_trigger_x_scale) &&
		inScopeY(target, m_sharedmemory->s_data.auto_trigger_up_scale, m_sharedmemory->s_data.auto_trigger_down_scale) &&
		coolDownTime(auto_start, m_sharedmemory->s_data.delay_base, m_sharedmemory->s_data.delay_delay)) {	//! 冷却
		//! 监听扳机按键
		if (m_mouse->monitor(m_sharedmemory->s_data.auto_key)) {
			m_mouse->trigger();
		}
		auto_start = std::chrono::system_clock::now();  	//! 更新下一个CD
	}
}

void Functional::autoTrigger(TargetInfo* target) {

	//! 扳机方式
	switch (m_sharedmemory->s_data.auto_model) {
#pragma region 移动扳机
	case 0:
		if (m_mouse->monitor(m_sharedmemory->s_data.aim_key)) {
			//! 移动
			m_mouse->move(static_cast<int>(target->move_x), static_cast<int>(target->move_y));
			//! 如果在范围内，执行扳机
			executeTrigger(target);
		}
		break;
#pragma endregion comment
#pragma region 架点扳机
	case 1:
		//! 如果在范围内，执行扳机
		executeTrigger(target);
		break;
#pragma endregion comment
#pragma region 延迟扳机
	case 2:
		if (m_mouse->monitor(m_sharedmemory->s_data.aim_key)) {
			m_mouse->move(static_cast<int>(target->move_x), static_cast<int>(target->move_y));
			//! 在CD内
			if (coolDownTime(sleep_start, m_sharedmemory->s_data.delay_base, m_sharedmemory->s_data.delay_delay)) {
				//! 如果在范围内，执行扳机
				if (inScopeX(target, m_sharedmemory->s_data.auto_trigger_x_scale) &&
					inScopeY(target, m_sharedmemory->s_data.auto_trigger_up_scale, m_sharedmemory->s_data.auto_trigger_down_scale)) {	//! 冷却
					if (m_mouse->monitor(m_sharedmemory->s_data.auto_key)) {
						m_mouse->trigger();
					}
					sleep_start = std::chrono::system_clock::now();		//! 更新下一个延迟CD
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
	//! 更新时间点
	if (!m_mouse->monitor(m_sharedmemory->s_data.aim_key)) {
		sleep_start = std::chrono::system_clock::now();
	}

	//! 跳过空
	if (m_process->indices.size() == 0) {
		return;
	}

	std::vector<float> distance;			//! 欧式距离
	std::vector<int> new_indices;			//! 新索引表

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
	if (idx == -1) {return;}		//! -1 = 没有符合的目标
	
	//! 获取最近目标的信息
	TargetInfo target{};
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
		target.center_y = (target.origin_y + target.height) - (target.height * m_sharedmemory->s_data.aim_position);
	}

	//! 相对距离
	target.distance_x = (m_point->origin_x + target.center_x) - m_point->center_x;
	target.distance_y = (m_point->origin_y + target.center_y) - m_point->center_y;

	//! 范围跳过
	if (abs(target.distance_x) > m_sharedmemory->s_data.aim_range * 0.5 || abs(target.distance_y) > m_sharedmemory->s_data.aim_range * 0.5) return;

	//! 控制计算(使用对象，函数式（低配机器），多线程（高配机器）)
	target.move_x = control->control_x(target.distance_x, m_sharedmemory->s_data);
	target.move_y = control->control_y(target.distance_y, m_sharedmemory->s_data);

	//! 最大移动
	target.move_x = MaxMovePixel(target.move_x, m_sharedmemory->s_data.max_pixel);
	target.move_y = MaxMovePixel(target.move_y, m_sharedmemory->s_data.max_pixel);

	//! 扳机移动 or 单移动
	(m_sharedmemory->s_signal.auto_trigger == true) ? autoTrigger(&target) : onlyMcove(&target);

	//! 笔记
	//! - 考虑增加控制类，以便扩展其他控制算法
	//! - 范围判定存在bug，放弃使用相对距离进行判断，使用屏幕中心做计算
	//! - 考虑移除fov算法，该算法实用价值不高
	//! - 尝试使用头文件分布声明共享内存
	//! - 考虑优化yolo基类，以便增加非yolo系的框架（如预处理的归一化，模型的检查(增加States状态)，后处理）
	//! - 考虑增加推理扩展模块（类obs的插件注册），防止依赖臃肿
}

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

	//! 初始化控制 control
	control = sf::createControlObj(m_control_manner);
	if (m_mouse == nullptr) {
		std::cout << "[debug]: 创建control对象失败" << std::endl;
		return IStates(false, "创建control对象失败");
	}

	//! control 初始化
	hr = control->init();
	if (hr.is_error()) {
		std::cout << "[debug]: control对象init失败" << std::endl;
		return hr;
	}
	return IStates();
}

void Functional::Release() {
	m_mouse->close();
	control->Release();
	delete this;                //! 等价于调用析构
}

Functional::Functional(LockInfo info) :LOCK(info) {
	std::cout << "[debug]: Functional 构造" << std::endl;
}

Functional::~Functional() {
	std::cout << "[debug]: Functional 析构" << std::endl; 
}
