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

IStates IEasyKeyMouse::close() {
    if (INVALID_HANDLE_VALUE != easy_key_mouse_handle) {
        M_Close(easy_key_mouse_handle);
    }
    return IStates(true,"�׼����ͷ����");
}
