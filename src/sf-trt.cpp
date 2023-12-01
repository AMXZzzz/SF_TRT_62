/*
* ����������
* ������ǿ�Դ��������������ʹ�á��޸ĺͷ�������ʹ�� GNU ͨ�ù������֤�汾 2��GPLv2�������
* ��ע�⣬�ڸ���GPLv2���������֮ǰ�������������������
* 1. �κ��޸ĺ�������Ʒ����ͬ������GPLv2����������������Ʒ���ṩGPLv2�������ı���
* 2. �����뱣��ԭʼ����İ�Ȩͨ�棬����������Ŀ����ȷָ��ԭʼ���ߺ��������Դ��
* 3. ������ַ���������������ṩ������Դ���룬������GPLv2��Ҫ������������޸ĺ�������Ʒ��Դ���롣
* 4. ������ǰ�"ԭ��"�ṩ�ģ��������κ���ʾ��ʾ�ı�֤�����߶���ʹ�ñ������ɵ��κ�ֱ�ӡ���ӡ�ż�������⡢���ͻ�ͽ����𺦲��е����Ρ�
* ʹ�ô˴��뼴��ʾ��ͬ������GPLv2������Ҫ�������������������
* ����ϸ�Ķ�GNUͨ�ù������֤�汾2���˽������ϸ��Ϣ���������� http ://www.gnu.org/licenses/gpl-2.0.html ���ҵ�GPLv2�������ı���
* [��������]: bilibili������Ϣ
* [��ַ]:https://github.com/AMXZzzz/SF_TRT_62.git
* [����]: 2023/10/26
*/
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
	sharedmemory->s_info.frame_type = 0;										//! ������
	sharedmemory->s_info.yolo_tyoe =  0;										//! yolo����
	sharedmemory->s_data.conf =  0.3;											//! ���Ŷ�
	sharedmemory->s_data.iou =  0.1;											//! iou���Ŷ�
	sharedmemory->s_info.model_path = "ckbq_yolov5n_2w5.engine";				//! ģ��·��

	//! ����ѭ��
	while (sharedmemory->s_signal.dll_exit_signal == false) {

		//! ����AI�߳��ź�
		if (sharedmemory->s_signal.ai_start_signal == true) {
			ac.start();	//! ����AI
			sharedmemory->s_signal.ai_start_signal = false;	//! ��λ�źţ���Ȼһֱ�ڴ����߳�
		}

		//! �������˳�dll�ź�
		if (sharedmemory->s_signal.dll_exit_signal == true) {
			sharedmemory->s_signal.dll_exit_signal = false;		//! ��λ�ź�,��Ȼִ�е�����Զ��˳�
			ac.exit();		//! ʹִ�����߳��˳�
			ac.join();		//! �ȴ�ִ�����˳�
			//! �ͷ�
			ac.Release();
			//! ж������dll�������
		}
	}
}

//! ����
int main() {
	test();
	return 0;
}