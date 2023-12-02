#include "lock_functional.h"


IStates Functional::initLock() {

	//! ����mousec����
	m_mouse = sf::createMouse(&m_mouse_info);
	if (m_mouse == nullptr) {
		std::cout << "[debug]: ����Mouse����ʧ��" << std::endl;
		return IStates(false, "����Mouse����ʧ��");
	}
	//! mousec��ʼ��
	IStates hr = m_mouse->init();
	if (hr.is_error()) {
		std::cout << "[debug]: Mouse����initʧ��" << std::endl;
		return hr;
	}

	//! ��ʼ������
	return IStates();
}

void Functional::action() {
	//! �ƶ��߼�
	
	m_mouse->move(1, 1);




}

void Functional::Release() {
	delete this;                //! �ȼ��ڵ�������
}
