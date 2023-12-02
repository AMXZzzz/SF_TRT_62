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
	sf::Type::MousecType mousec_manner;		//! �ƶ���ʽ
	void* data;
};

class IMouse {
public:
	IMouse() { std::cout << "[debug]: IMouse ���๹��" << std::endl; }
	//! ��ʼ���ƶ�
	virtual IStates init() = 0;
	//! ��������
	virtual bool monitor(int key)= 0;
	//! ִ������ƶ�
	virtual IStates move(int x,int y) = 0;
	//! ִ�а������
	virtual void trigger() = 0;
	//! �ͷ���Դ
	virtual IStates close() = 0;
	//! �����黯
	virtual ~IMouse() { std::cout << "[debug]: IMouse ��������" << std::endl; };
private:

};

namespace sf {
	IMouse* createMouse(MouseInfo* info);
}
