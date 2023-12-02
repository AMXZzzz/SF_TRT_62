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
#include "mouse_ghub.h"
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

