#include <windows.h>
#include "move-way.h"
#include <iostream>
#pragma region 罗技Ghub
BOOL MoveWay::GetNTStatus(PCWSTR device_name) {
	UNICODE_STRING name;
	OBJECT_ATTRIBUTES attr{};
	RtlInitUnicodeString(&name, device_name);
	InitializeObjectAttributes(&attr, &name, 0, NULL, NULL);

	//打开Net
	LG_status = NtCreateFile(&Lg, GENERIC_WRITE | SYNCHRONIZE, &attr, &LG_io, 0,
		FILE_ATTRIBUTE_NORMAL, 0, 3, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);
	if (!(NT_SUCCESS(LG_status))) {
		return FALSE;
	}
	return TRUE;
}

VOID MoveWay::SendDrive(int x, int y) {
	mouse_io.x = x;
	mouse_io.y = y;
	IO_STATUS_BLOCK block;
	NtDeviceIoControlFile(Lg, 0, 0, 0, &block, 0x2a2010, &mouse_io, sizeof(MOUSE_IO), 0, 0);	// == 0L is free
}

VOID LGMove(int x, int y) {
	if (abs(x) > 127 || abs(y) > 127) {
		int x_left = x; int y_left = y;

		if (abs(x) > 127) {
			MoveWay::Get().SendDrive(int(x / abs(x)) * 127, 0);
			x_left = x - int(x / abs(x)) * 127;
		}
		else {
			MoveWay::Get().SendDrive(int(x), 0);
			x_left = 0;
		}

		if (abs(y) > 127) {
			MoveWay::Get().SendDrive(0, int(y / abs(y)) * 127);
			y_left = y - int(y / abs(y)) * 127;
		}
		else {
			MoveWay::Get().SendDrive(0, int(y));
			y_left = 0;
		}

		return LGMove(x_left, y_left);
	}
	else {
		MoveWay::Get().SendDrive(x, y);
	}
}

VOID LGMoveClose() {
	if (MoveWay::Get().Lg != NULL) {
		NtClose(MoveWay::Get().Lg);
		MoveWay::Get().Lg = nullptr;
	}
}

BOOL MoveWay::InitLogitech() {

	if (Lg == NULL) {

		wchar_t buffer0[] = L"\\??\\ROOT#SYSTEM#0002#{1abc05c0-c378-41b9-9cef-df1aba82b015}";
		if (GetNTStatus(buffer0)) {
			return FALSE;
		}

		wchar_t buffer1[] = L"\\??\\ROOT#SYSTEM#0001#{1abc05c0-c378-41b9-9cef-df1aba82b015}";
		if (GetNTStatus(buffer1)) {
			return FALSE;
		}

		wchar_t buffer3[] = L"\\??\\ROOT#SYSTEM#0003#{1abc05c0-c378-41b9-9cef-df1aba82b015}";
		if (GetNTStatus(buffer3)) {
			return FALSE;
		}
	}

	MoveR = LGMove;
	MoveClose = LGMoveClose;
	return TRUE;
}
#pragma endregion

#pragma region 易键鼠

#pragma endregion

#pragma region SendInput
VOID SendMove(int x, int y) {
	std::cout << "SendMove:  x " << x << std::endl;
	std::cout << "SendMove:  y " << y << std::endl;
	MoveWay::Get().send_input.mi.dx = x;
	MoveWay::Get().send_input.mi.dy = y;
	SendInput(1, &MoveWay::Get().send_input, sizeof(INPUT));
}

VOID SendMoveClose() {
}

VOID MoveWay::InitSendInput() {
	send_input.type = INPUT_MOUSE;
	send_input.mi.dx = 0;
	send_input.mi.dy = 0;
	send_input.mi.mouseData = 0;
	send_input.mi.dwFlags = MOUSEEVENTF_MOVE;   //MOUSEEVENTF_ABSOLUTE 代表决对位置  MOUSEEVENTF_MOVE代表移动事件
	send_input.mi.time = 0;
	send_input.mi.dwExtraInfo = 0;

	MoveR = SendMove;
	MoveClose = SendMoveClose;
}
#pragma endregion

#pragma region 自定义移动方式

#pragma endregion

BOOL MoveWay::InitMove(int way) {
	switch (way) {
	case 0:		// ghub
		if (!InitLogitech()) {
			return FALSE;
		}
		break;
	case 1:		// SendInput
		InitSendInput();
		break;
	}
	return TRUE;
}


