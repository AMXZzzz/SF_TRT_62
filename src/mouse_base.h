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


class IMouse {
public:
	IMouse();
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
	virtual ~IMouse();
private:

};

struct MouseInfo {
	sf::Type::MousecType mousec_manner;		//! �ƶ���ʽ
	void* data;
};

namespace sf {
	IMouse* createMouse(MouseInfo* info);
}
