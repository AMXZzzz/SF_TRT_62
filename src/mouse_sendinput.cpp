#include "mouse_sendinput.h"


IStates ISendInput::init() {
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.dwExtraInfo = 0;
    input.mi.time = 0;
    input.mi.dwFlags = MOUSEEVENTF_MOVE; //MOUSEEVENTF_ABSOLUTE �������λ��  MOUSEEVENTF_MOVE��������ƶ�
    return IStates(true,"Seninput ��ʼ������");
}

IStates ISendInput::move(int x, int y) {
    input.mi.dx = x;
    input.mi.dy = y;
    if (SendInput(1, &input, sizeof(input)) == 0) {
        return IStates(false, "SendInput ִ��ʧ��");
    }
}

IStates ISendInput::close() {
    return IStates(true,"SenInput close done");
}
