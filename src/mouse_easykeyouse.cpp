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
#include "mouse_easykeyouse.h"
#include "msdk.h"

IStates IEasyKeyMouse::init() {
    easy_key_mouse_handle = M_Open(1);
    if (INVALID_HANDLE_VALUE == easy_key_mouse_handle) {
        return IStates(false, "���׼���ʧ��");
    }
    return IStates();
}

IStates IEasyKeyMouse::move(int x, int y) {
    M_MoveR(easy_key_mouse_handle, x, y);
    return IStates();
}

bool IEasyKeyMouse::monitor(int key) {
    return GetAsyncKeyState(key);
}

void IEasyKeyMouse::trigger() {
    M_LeftClick(easy_key_mouse_handle, 1);
}

IStates IEasyKeyMouse::close() {
    if (INVALID_HANDLE_VALUE != easy_key_mouse_handle) {
        M_Close(easy_key_mouse_handle);
    }
    delete this;
    return IStates(true, "�׼����ͷ����");
}

IEasyKeyMouse::IEasyKeyMouse() {
    std::cout << "[debug]: IEasyKeyMouse ����" << std::endl;
}

IEasyKeyMouse::~IEasyKeyMouse() {
    std::cout << "[debug]: IEasyKeyMouse ����" << std::endl;
}
