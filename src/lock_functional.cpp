#include "lock_functional.h"

bool Functional::initLock() {

    //! 初始化mousec对象
    m_mouse = sf::createMouse(&m_mouse_info);
    if (m_mouse == nullptr) {
        std::cout << "[debug]: 创建Mouse对象失败" << std::endl;
        m_states.push_back(IStates(false,"创建Mouse对象失败"));
        return false;
    }
    //! 鼠标对象初始化
    IStates hr = m_mouse->init();
    if (hr.is_error()) {
        std::cout << "[debug]: Mouse对象init失败" << std::endl;
        m_states.push_back(hr);
        return false;
    }

    //! 初始化其他

    return true;
}

IStates Functional::getLastError() {
    if (m_states.empty()) {
        return IStates(true, "Functional对象未产生任何错误");
    }
    int idx = m_states.size() - 1;
    return m_states[idx];
}

void Functional::action() {
    m_mouse->move(10, 10);
}

void Functional::Release() {
    delete this;                //! 等价于调用析构
}
