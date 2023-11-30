#include "Actuator.h"


#define MAX_SIZE 1024						//! �����ڴ��С
#define MapFileName "sf_61_shared_memory"   //! �����ڴ�ID

//! ��ʼ�������ڴ�
bool initmem(SharedMemory** signal) {
	HANDLE MapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SIZE, MapFileName);
	if (MapFile == NULL) {
		return false;
	}

	*signal = (SharedMemory*)MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE);
	if (*signal == NULL) {
		return false;
	}
	
	return true;
}

//! �����obs�����
void test() {
	//! ��ʼ�������ڴ�
	SharedMemory* sharedmemory;
	if (!initmem(&sharedmemory)) return;  //! һ��Ҫ��UI�˳�ʼ�������ڴ棬�����˳�

	//! ����ִ����
	Actuator ac(sharedmemory);


	//! �����ڴ洴��ʱ��ֵ�ǲ���ֵ����signal->ai_start_signal ������false,Ҳ������true
	//! �̶�ֵ�����ԣ�ui���Ѿ���ʼ�������ڴ棬��������ʼ��
	sharedmemory->s_signal.dll_exit_signal = false;								//! dll �˳��ź�
	sharedmemory->s_signal.ai_start_signal = true;								//! ִ������ʼ�ź�
	sharedmemory->s_signal.show_detect_window = true;							//! ��ʾ��ⴰ���ź�
	sharedmemory->s_info.frame_type = 1;										//! ������
	sharedmemory->s_info.yolo_tyoe =  0;										//! yolo����
	sharedmemory->s_data.conf =  0.3;											//! ���Ŷ�
	sharedmemory->s_data.iou =  0.1;											//! iou���Ŷ�
	sharedmemory->s_info.model_path = "cf_yolov5s_15w_640_2label.onnx";			//! ģ��·��

	//! ����ѭ��
	while (sharedmemory->s_signal.dll_exit_signal == false) {

		//! ����AI�߳�
		if (sharedmemory->s_signal.ai_start_signal == true) {
			ac.start();	//! ����AI
			sharedmemory->s_signal.ai_start_signal = false;	//! ��λ�źţ���Ȼһֱ�ڴ����߳�
		}

		//! �������˳�dll�ź�
		if (sharedmemory->s_signal.dll_exit_signal == true) {
			sharedmemory->s_signal.dll_exit_signal = false;		//! ��λ�ź�,��Ȼִ�е�����Զ��˳�
			ac.exit();		//! ʹִ�����߳��˳�
			ac.join();		//! �ȴ�ִ�����˳�
			//! ж������dll�������
			
			//! �ͷŹ����ڴ�,
			
		}
	}
}

//! ����
int main() {
	test();
	return 0;
}