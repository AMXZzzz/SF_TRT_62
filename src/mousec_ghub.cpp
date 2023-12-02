#include "mousec_ghub.h"
#include <winternl.h>

typedef struct {	// 声明类型
    char button = 0;
    char x = 0;
    char y = 0;
    char wheel = 0;
    char unk1 = 0;
} MOUSE_IO;

static IStates getNtStatus(HANDLE handle, PCWSTR device_name) {
    UNICODE_STRING name;
    OBJECT_ATTRIBUTES attr{};
    RtlInitUnicodeString(&name, device_name);
    InitializeObjectAttributes(&attr, &name, 0, NULL, NULL);

    IO_STATUS_BLOCK io;
    NTSTATUS hr =  NtCreateFile(&handle,
        GENERIC_WRITE | SYNCHRONIZE,
        &attr, &io, 0,
        FILE_ATTRIBUTE_NORMAL, 0, 3, 
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, 
        0, 0);
    if (!NT_SUCCESS(hr)) {
        return IStates(false, "创建Nt驱动对象失败");
    }
    return IStates();
}

static void moveOrderToDrive(HANDLE handle,int x,int y){
    MOUSE_IO io;
    io.x = x;
    io.y = y;
    IO_STATUS_BLOCK block;
    NtDeviceIoControlFile(handle, 0, 0, 0, &block, 0x2a2010, &io, sizeof(io), 0, 0);
}

static void clickOrderToDrive(HANDLE handle,int key){
    MOUSE_IO io;
    io.button = key;
    IO_STATUS_BLOCK block;
    NtDeviceIoControlFile(handle, 0, 0, 0, &block, 0x2a2010, &io, sizeof(io), 0, 0);
}

IStates IGHUB::init() {
    wchar_t buffer0[] = L"\\??\\ROOT#SYSTEM#0002#{1abc05c0-c378-41b9-9cef-df1aba82b015}";
    if (getNtStatus(ghub_handle, buffer0).is_success()) {
        return IStates();
    }

    wchar_t buffer1[] = L"\\??\\ROOT#SYSTEM#0001#{1abc05c0-c378-41b9-9cef-df1aba82b015}";
    if (getNtStatus(ghub_handle, buffer0).is_error()) {
        return IStates();
    }      
    wchar_t buffer2[] = L"\\??\\ROOT#SYSTEM#0003#{1abc05c0-c378-41b9-9cef-df1aba82b015}";
    if (getNtStatus(ghub_handle, buffer0).is_error()) {
        return IStates();
    } 
    return IStates(false, "打开GHUB驱动失败,开机启动一次HUB,若无效.检查GHUB版本");
}

IStates IGHUB::move(int x, int y) {
    if (abs(x) > 127 || abs(y) > 127) {
        int x_left = x; 
        int y_left = y;
        if (abs(x) > 127) {
            moveOrderToDrive(ghub_handle, int(x / abs(x)) * 127, 0);
            x_left = x - int(x / abs(x)) * 127;
        }
        else {
            moveOrderToDrive(ghub_handle, x, 0);
            x_left = 0;
        }
        if (abs(x) > 127) {
            moveOrderToDrive(ghub_handle, 0,int(y / abs(y)) * 127);
            y_left = y - int(y / abs(y)) * 127;
        }
        else {
            moveOrderToDrive(ghub_handle, 0,y);
            y_left = 0;
        }
        return move(x_left, y_left);
    }
    else {
        moveOrderToDrive(ghub_handle, x, y);
    }
    return IStates();
}

bool IGHUB::monitor(int key) {
    return GetAsyncKeyState(key);
}

void IGHUB::trigger() {
    clickOrderToDrive(ghub_handle,1);       //! 激活左键
    clickOrderToDrive(ghub_handle, 0);      //! 释放左键
}


IStates IGHUB::close() {
    if (ghub_handle != NULL) {
        NtClose(ghub_handle);
        ghub_handle = nullptr;
    }
    return IStates(true,"罗技驱动释放成功");
}

