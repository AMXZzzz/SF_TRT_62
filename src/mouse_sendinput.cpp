#include "mouse_sendinput.h"

IStates ISendInput::init() {
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.dwExtraInfo = 0;
    input.mi.time = 0;
    input.mi.dwFlags = MOUSEEVENTF_MOVE; //MOUSEEVENTF_ABSOLUTE 代表决对位置  MOUSEEVENTF_MOVE代表相对移动
    return IStates(true,"Seninput 初始化正常");
}

IStates ISendInput::move(int x, int y) {
    input.mi.dx = x;
    input.mi.dy = y;
    if (SendInput(1, &input, sizeof(input)) == 0) {
        return IStates(false, "SendInput 执行失败");
    }
}

void ISendInput::trigger() {
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;   //MOUSEEVENTF_LEFTDOWN 左键按下
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;
    SendInput(1, &input, sizeof(INPUT));

    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;   // MOUSEEVENTF_LEFTUP  左键松开
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;
    SendInput(1, &input, sizeof(INPUT));
}

bool ISendInput::monitor(int key) {
    return GetAsyncKeyState(key);
}

IStates ISendInput::close() {
    return IStates(true,"SenInput close done");
}

