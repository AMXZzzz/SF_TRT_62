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
#include "lock_base.h"
#include "lock_functional.h"

LOCK* sf::createLockObject(LockInfo info) {
	switch (info.lock_manner) {
	case sf::Type::LockManner::Sync: return new Functional(info);
	case sf::Type::LockManner::Async: return new Functional(info);
	}
    return nullptr;
}

LOCK::LOCK(LockInfo info) : m_point(info.point), m_mouse_info(info.mouse_info), m_process(info.process),
	m_sharedmemory(info.sharedmemory), m_control_manner(info.control_manner) {
	std::cout << "[debug]: LOCK ���๹��" << std::endl;
	if (m_process == nullptr) { std::cout << "[debug]: ����LOCK��processָ��Ϊ��" << std::endl; }
	if (m_sharedmemory == nullptr) { std::cout << "[debug]: ����LOCK��sharedmemoryָ��Ϊ��" << std::endl; }
}

LOCK::~LOCK() {
	std::cout << "[debug]: LOCK ��������" << std::endl;
}
