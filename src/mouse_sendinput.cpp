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
#include "mouse_sendinput.h"

IStates ISendInput::init() {
    move_input.type = INPUT_MOUSE;
    move_input.mi.mouseData = 0;
    move_input.mi.dwExtraInfo = 0;
    move_input.mi.time = 0;
    move_input.mi.dwFlags = MOUSEEVENTF_MOVE; //MOUSEEVENTF_ABSOLUTE �������λ��  MOUSEEVENTF_MOVE��������ƶ�
    return IStates(true,"Seninput ��ʼ������");
}

IStates ISendInput::move(int x, int y) {
    move_input.mi.dx = x;
    move_input.mi.dy = y;
 
    if (SendInput(1, &move_input, sizeof(move_input)) == 0) {
        return IStates(false, "SendInput ִ��ʧ��");
    }
    return IStates();   //! ����return
}

void ISendInput::trigger() {
    INPUT click_input{};
    click_input.type = INPUT_MOUSE;
    click_input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;   //MOUSEEVENTF_LEFTDOWN �������
    click_input.mi.time = 0;
    click_input.mi.dwExtraInfo = 0;
    SendInput(1, &click_input, sizeof(INPUT));

    click_input.type = INPUT_MOUSE;
    click_input.mi.dwFlags = MOUSEEVENTF_LEFTUP;   // MOUSEEVENTF_LEFTUP  ����ɿ�
    click_input.mi.time = 0;
    click_input.mi.dwExtraInfo = 0;
    SendInput(1, &click_input, sizeof(INPUT));
}

bool ISendInput::monitor(int key) {
    return GetAsyncKeyState(key);
}

IStates ISendInput::close() {
    delete this;
    return IStates(true,"SenInput close done");
}

ISendInput::ISendInput() {
    std::cout << "[debug]: ISendInput ����" << std::endl;
}

ISendInput::~ISendInput() {
    std::cout << "[debug]: ISendInput ����" << std::endl;
}
