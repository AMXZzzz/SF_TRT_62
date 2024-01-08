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
#include "base_type.h"
#include "control_base.h"


namespace sf {
	namespace Type {
		enum LockManner :int {
			Sync = 0,
			Async = 1,
		};
	}
}

struct TargetInfo {		//! ���Ŀ����Ϣ
	float origin_x;		//! Ŀ��boxԭ��x
	float origin_y;		//! Ŀ��boxԭ��y
	float width;		//! Ŀ��box��
	float height;		//! Ŀ��box��
	float center_x;		//! Ŀ��box���ĵ�x
	float center_y;		//! Ŀ��box���ĵ�x
	float distance_x;	//! Ŀ����Ծ���x,ԭʼ����
	float distance_y;	//! Ŀ����Ծ���y��ԭʼ����
	float move_x;		//! Ŀ��x��Ҫ�ƶ��ľ��룬�������
	float move_y;		//! Ŀ��x��Ҫ�ƶ��ľ��룬�������
};

struct LockInfo {
	sf::Type::LockManner lock_manner;	//! ����ķ�ʽ
	sf::Type::ControlManner control_manner;
	MouseInfo mouse_info;			//! ���ִ�з�ʽ
	Process* process;				//! ����ṹ��
	SharedMemory* sharedmemory;		//! �����ڴ�ָ��
	IPoint* point;					//! �����ָ��
};

class  LOCK {
public:
	LOCK(LockInfo info);
	//! ��ʼ��lock 
	virtual IStates initLock() = 0;
	//! ִ������
	virtual void action() = 0;
	//! �ͷ�
	virtual void Release() = 0;
	//! �����ϰ����
	virtual ~LOCK();
protected:
	sf::Type::ControlManner m_control_manner;
	IPoint* m_point;				//! �����
	Control* control;				//! ���ƶ���
	IMouse* m_mouse;				//! ������
	Process* m_process;				//! �����
	MouseInfo m_mouse_info{};		//! �������

	SharedMemory* m_sharedmemory;	//! �����ڴ棬�Ż����ź�
private:
	LOCK();	//! ����Ĭ�Ϲ���
};

namespace sf {
	LOCK* createLockObject(LockInfo info);
}

