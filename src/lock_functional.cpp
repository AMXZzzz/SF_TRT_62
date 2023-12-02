#include "lock_functional.h"

bool Functional::initLock() {

    //! ��ʼ��mousec����
    m_mouse = sf::createMouse(&m_mouse_info);
    if (m_mouse == nullptr) {
        std::cout << "[debug]: ����Mouse����ʧ��" << std::endl;
        m_states.push_back(IStates(false,"����Mouse����ʧ��"));
        return false;
    }
    //! �������ʼ��
    IStates hr = m_mouse->init();
    if (hr.is_error()) {
        std::cout << "[debug]: Mouse����initʧ��" << std::endl;
        m_states.push_back(hr);
        return false;
    }

    //! ��ʼ������

    return true;
}

IStates Functional::getLastError() {
    if (m_states.empty()) {
        return IStates(true, "Functional����δ�����κδ���");
    }
    int idx = m_states.size() - 1;
    return m_states[idx];
}

void Functional::action() {
    m_mouse->move(10, 10);
}

void Functional::Release() {
    delete this;                //! �ȼ��ڵ�������
}
