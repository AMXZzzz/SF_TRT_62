#include "Actuator.h"


#define MAX_SIZE 1024
#define MapFileName "sf_61_shared_memory"

bool initmem(MemorySignal** signal) {
	HANDLE MapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SIZE, MapFileName);
	if (MapFile == NULL) {
		return false;
	}

	*signal = (MemorySignal*)MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE);
	if (*signal == NULL) {
		return false;
	}

	//! �����ڴ洴��ʱ��ֵ�ǲ���ֵ����signal->ai_start_signal ������false,Ҳ������true
	//! ����ʹ���ַ������ж��Ƿ��ʼ���������ui��δ��ʼ��������dll�˳�ʼ��
	//! ���ui���Ѿ���ʼ�������ڴ棬��������ʼ��
	if ((*signal)->shared_memory_succecc) {
		(*signal)->conf = 0.3;
		(*signal)->iou = 0.1;
		(*signal)->ai_start_signal = false;
		(*signal)->dll_exit_signal = false;
		(*signal)->shared_memory_succecc = true;
	}
	return true;
}

//! �����obs�����
void test() {
	//! ��ʼ�������ڴ�
	MemorySignal* signal;
	if (!initmem(&signal)) return;

	//! ����ִ����
	Actuator ac(signal);

	//! ����ѭ��
	while (signal->dll_exit_signal == false) {

		//! ����AI�߳�
		if (signal->ai_start_signal == true) {
			ac.start();	//! ����AI
			signal->ai_start_signal = false;	//! ��λ�źţ���Ȼһֱ�ڴ����߳�
		}

		//! �������˳�dll�ź�
		if (signal->dll_exit_signal == true) {
			signal->dll_exit_signal = false; //! ��λ�ź�,��Ȼ����dllִ�е�����Զ��˳�
			ac.exit();		//! ʹִ�����߳��˳�
			ac.join();		//! �ȴ�ִ�����˳�
			//! ж������dll�������
			
			//! �ͷŹ����ڴ�
		}
	}
}

//! ����
int main() {
	test();

	return 0;
}