#pragma once
#include "IState.h"

namespace sf {
	namespace Type {
		enum MousecType :int {
			GHUB = 0,
			EasyKeyMouse = 1,
			SendInput = 2,
		};
	}
}

struct MouseInfo {
	sf::Type::MousecType mousec_manner;		//! 移动方式
	void* data;
};

class IMouse {
public:
	IMouse() { std::cout << "[debug]: IMouse 基类构造" << std::endl; }
	//! 初始化移动
	virtual IStates init() = 0;
	//! 监听按键
	virtual bool monitor(int key)= 0;
	//! 执行相对移动
	virtual IStates move(int x,int y) = 0;
	//! 执行扳机单击
	virtual void trigger() = 0;
	//! 释放资源
	virtual IStates close() = 0;
	//! 析构虚化
	virtual ~IMouse() { std::cout << "[debug]: IMouse 基类析构" << std::endl; };
private:

};

namespace sf {
	IMouse* createMouse(MouseInfo* info);
}
