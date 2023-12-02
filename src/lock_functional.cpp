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

void Functional::action() {
	//! 移动逻辑
	
	m_mouse->move(1, 1);




}

void Functional::Release() {
	delete this;                //! 等价于调用析构
}
