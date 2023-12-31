/*
* 免责声明：
* 本软件是开源程序，您可以自由使用、修改和发布它，使用 GNU 通用公共许可证版本 2（GPLv2）的条款。
* 请注意，在根据GPLv2发布本软件之前，您必须遵守以下条款：
* 1. 任何修改和派生作品必须同样采用GPLv2，并在您的派生作品中提供GPLv2的完整文本。
* 2. 您必须保留原始代码的版权通告，并在您的项目中明确指明原始作者和软件的来源。
* 3. 如果您分发本软件，您必须提供完整的源代码，以满足GPLv2的要求。这包括您的修改和派生作品的源代码。
* 4. 本软件是按"原样"提供的，不附带任何明示或暗示的保证。作者对于使用本软件造成的任何直接、间接、偶发、特殊、典型或惩戒性损害不承担责任。
* 使用此代码即表示您同意遵守GPLv2的所有要求和免责声明的条件。
* 请仔细阅读GNU通用公共许可证版本2以了解更多详细信息。您可以在 http ://www.gnu.org/licenses/gpl-2.0.html 上找到GPLv2的完整文本。
* [作者姓名]: bilibili：随风而息
* [地址]:https://github.com/AMXZzzz/SF_TRT_62.git
* [日期]: 2023/10/26
*/
#include "mouse_sendinput.h"

IStates ISendInput::init() {
    move_input.type = INPUT_MOUSE;
    move_input.mi.mouseData = 0;
    move_input.mi.dwExtraInfo = 0;
    move_input.mi.time = 0;
    move_input.mi.dwFlags = MOUSEEVENTF_MOVE; //MOUSEEVENTF_ABSOLUTE 代表决对位置  MOUSEEVENTF_MOVE代表相对移动
    return IStates(true,"Seninput 初始化正常");
}

IStates ISendInput::move(int x, int y) {
    move_input.mi.dx = x;
    move_input.mi.dy = y;
 
    if (SendInput(1, &move_input, sizeof(move_input)) == 0) {
        return IStates(false, "SendInput 执行失败");
    }
    return IStates();   //! 必须return
}

void ISendInput::trigger() {
    INPUT click_input{};
    click_input.type = INPUT_MOUSE;
    click_input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;   //MOUSEEVENTF_LEFTDOWN 左键按下
    click_input.mi.time = 0;
    click_input.mi.dwExtraInfo = 0;
    SendInput(1, &click_input, sizeof(INPUT));

    click_input.type = INPUT_MOUSE;
    click_input.mi.dwFlags = MOUSEEVENTF_LEFTUP;   // MOUSEEVENTF_LEFTUP  左键松开
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
    std::cout << "[debug]: ISendInput 构造" << std::endl;
}

ISendInput::~ISendInput() {
    std::cout << "[debug]: ISendInput 析构" << std::endl;
}
