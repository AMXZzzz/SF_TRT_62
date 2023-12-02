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
    return IStates(true, "易键鼠释放完成");
}
