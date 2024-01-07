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
bool initmem(SharedMemory** sharedmemory) {
	HANDLE MapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SIZE, MapFileName);
	if (MapFile == NULL) {
		return false;
	}

	*sharedmemory = (SharedMemory*)MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE);
	if (*sharedmemory == NULL) {
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
	sharedmemory->s_info.yolo_tyoe = 0;											//! yolo����
	sharedmemory->s_info.frame_type = 1;										//! ������
	sharedmemory->s_info.equipment = 0;											//! �豸
	sharedmemory->s_info.lock_type = 0;											//! �������ʽ
	sharedmemory->s_info.mousec_type = 2;										//! ʹ��sendinput
	sharedmemory->s_info.model_path = "cf_yolov5s_15w_640_2label.onnx";			//! ģ��·��
	sharedmemory->s_info.logger_path = "";										//! ��־���·��

	sharedmemory->s_signal.dll_exit_signal = false;								//! dll �˳��ź�
	sharedmemory->s_signal.ai_start_signal = true;								//! ִ������ʼ�ź�
	sharedmemory->s_signal.show_detect_window = true;							//! ��ʾ��ⴰ���ź�
	sharedmemory->s_signal.category_filter = true;								//! ���ɸѡ
	sharedmemory->s_signal.first_class = true;									//! ��һ���
	sharedmemory->s_signal.second_class = false;								//! �ڶ����
	sharedmemory->s_signal.third_class = false;									//! �������
	sharedmemory->s_signal.fourth_class = false;								//! �������
	sharedmemory->s_signal.auto_trigger = true;									//! �Զ����
	sharedmemory->s_signal.keep_move = false;									//! �����ƶ�
		
	sharedmemory->s_data.conf =  0.4;											//! ���Ŷ�
	sharedmemory->s_data.iou =  0.1;											//! iou���Ŷ�
	sharedmemory->s_data.aim_position =  0.7;									//! ��׼λ��
	sharedmemory->s_data.aim_range =  640;										//! ��׼��Χ
	sharedmemory->s_data.aim_key = 0x02;										//! �������鰴��2
	sharedmemory->s_data.aim_key2 = 0x02;										//! �������鰴��3
	sharedmemory->s_data.auto_model = 2;										//! ���ģʽ
	sharedmemory->s_data.auto_key = 0x02;										//! �������鰴��
	sharedmemory->s_data.auto_key2 = 0x02;										//! �������鰴��2
	sharedmemory->s_data.auto_key3 = 0x02;										//! �������鰴��3
	sharedmemory->s_data.auto_trigger_x_scale =  1.0;							//! x�᷶Χ			
	sharedmemory->s_data.auto_trigger_up_scale =  1.0;							//! y ���ϲ�
	sharedmemory->s_data.auto_trigger_down_scale =  1.0;						//! y ���²�		

	sharedmemory->s_data.kp_x = 0.3;
	sharedmemory->s_data.ki_x = 0.1;
	sharedmemory->s_data.kd_x = 0.1;
	sharedmemory->s_data.kp_y = 0.3;
	sharedmemory->s_data.ki_y = 0.1;
	sharedmemory->s_data.kd_y = 0.1;

	sharedmemory->s_data.delay_base = 250;
	sharedmemory->s_data.delay_delay = 60;

	//! ����ѭ��
	while (sharedmemory->s_signal.dll_exit_signal == false) {
		//! ����AI�߳��ź�
		if (sharedmemory->s_signal.ai_start_signal == true) {
			ac.start();	//! ����AI
			sharedmemory->s_signal.ai_start_signal = false;	//! ��λ�źţ���Ȼһֱ�ڴ����߳�
		}

		//! �������˳�dll�ź�
		if (sharedmemory->s_signal.dll_exit_signal == true) {
			ac.exit();		//! ʹִ�����߳��˳�
			ac.join();		//! �ȴ�ִ�����˳�
			//! �ͷ�
			ac.Release();
			//! ��λ�ź�
			sharedmemory->s_signal.dll_exit_signal = false;		
			//! ж������dll�������
		}
	}
}

//! ����
int main() {
	test();
	return 0;
}


/*
	������
		Ϊÿ�������debug,
		���ע��
		�����־
*/