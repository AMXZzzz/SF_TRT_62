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
    return (SendInput(1, &input, sizeof(input)) == 0) ? IStates(false, "SendInput 执行失败") : IStates(true, "SendInput 成功");

    //SendInput(1, &input, sizeof(input));
    //return IStates();
}

IStates ISendInput::close() {
    return IStates(true,"SenInput close done");
}
