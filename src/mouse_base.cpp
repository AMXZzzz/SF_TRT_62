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
#include "mouse_base.h"
#include "mouse_sendinput.h"
#include "mouse_ghub.h"

IMouse* sf::createMouse(MouseInfo* info) {
	switch (info->mousec_manner) {
	case sf::Type::MousecType::SendInput: return new ISendInput();
	case sf::Type::MousecType::EasyKeyMouse: return nullptr;
	case sf::Type::MousecType::GHUB: return new IGHUB();
	}
	//! δ���о�Ϊ��
    return nullptr;
}

IMouse::IMouse() {
	std::cout << "[debug]: IMouse ���๹��" << std::endl;
}

IMouse::~IMouse() {
	std::cout << "[debug]: IMouse ��������" << std::endl; 
}
