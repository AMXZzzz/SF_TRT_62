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
#pragma once
#include "mouse_base.h"

namespace sf {
	namespace Type {
		enum LockManner :int {
			Functional = 0,
			Multithread = 1,
		};
	}
}

struct LockInfo {
	sf::Type::LockManner manner;	//! ����ķ�ʽ
	MouseInfo mouse_info;			//! ���ִ�з�ʽ
};

class  LOCK {
public:
	LOCK(MouseInfo mouseinfo): m_mouse_info(mouseinfo){std::cout << "[debug]: LOCK���๹��" << std::endl;}
	//! ��ʼ��lock 
	virtual bool initLock() = 0;
	//! ��ȡ������Ϣ
	virtual IStates getLastError() = 0;
	//! ִ������
	virtual void action() = 0;
	//! �ͷ�
	virtual void Release() = 0;
	virtual ~LOCK() { 
		m_mouse->close();
		std::cout << "[debug]: LOCK�����ͷ�" << std::endl;
	};
protected:
	IMouse* m_mouse;
	MouseInfo m_mouse_info{};
private:
	LOCK() {};	//! ����Ĭ�Ϲ���
};

namespace sf {
	LOCK* createLock(LockInfo* info);
}

