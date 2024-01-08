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
#include "mouse_easykeyouse.h"
#include "msdk.h"

IStates IEasyKeyMouse::init() {
    easy_key_mouse_handle = M_Open(1);
    if (INVALID_HANDLE_VALUE == easy_key_mouse_handle) {
        return IStates(false, "打开易键鼠失败");
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
    return IStates(true, "易键鼠释放完成");
}

IEasyKeyMouse::IEasyKeyMouse() {
    std::cout << "[debug]: IEasyKeyMouse 构造" << std::endl;
}

IEasyKeyMouse::~IEasyKeyMouse() {
    std::cout << "[debug]: IEasyKeyMouse 析构" << std::endl;
}
