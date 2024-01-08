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
#pragma once


#define DataVersion 6.2.0

//! ���������ָ��,ʵʱ��
struct Data {
	float conf = 0.3;							//! ���Ŷ�ָ��
	float iou = 0.1;							//! iou���Ŷ�ָ��
	float aim_position = 0.7;					//! ��׼λ��
	int aim_range = 200;						//! ���鷶Χ,����Ļ�м�Ϊ���ĵ� aim_range * aim_range������
	int aim_key = 0x01;							//! ���鴥����
	int aim_key2 = 0x01;						//! ���鴥����2
	int aim_key3 = 0x02;						//! ���鴥����3
	int auto_model = 0;							//! �����ʽ
	int auto_key = 0;							//! ������� key
	int auto_key2 = 0;							//! ������� key2
	int auto_key3 = 0;							//! ������� key3
	float auto_trigger_x_scale = 0.5;			//! �Զ�������Ҵ�����Χ����
	float auto_trigger_up_scale = 0.5;			//! �Զ�����ϲ�������Χ����
	float auto_trigger_down_scale = 0.5;		//! �Զ�����²�������Χ����

	float kp_x = 0.3;
	float ki_x = 0.1;
	float kd_x = 0.1;
	float kp_y = 0.3;
	float ki_y = 0.1;
	float kd_y = 0.1;
	int max_pixel = 15;							//! ����ƶ�����
	int delay_base = 150;
	int delay_delay = 60;

};

//! �����������Ϣ��������
struct Info {
	int yolo_tyoe = 0;						//! yolo���� 0��yolov5, 1��yolov8, 2��yolox
	int frame_type = 0;						//! ������� 0��tensrort, 1��dml
	int equipment = 0;						//! �豸���� 
	int lock_type = 0;						//! �����߼��ķ�ʽ 
	int mousec_type = 0;					//! �ƶ���ʽ 
	int control_model = 1;						//! ʹ�õĿ����㷨
	std::string model_path = "";			//! ģ��·��
	std::string logger_path = "";			//! ģ��·��

};

//! ������źţ�ʵʱ��
struct Signal {
	bool dll_exit_signal = false;			//! ����Ϊtrueʱ������dll�˳��ź�
	bool ai_start_signal = false;			//! ��Ϊtrueʱ������ai�߳�
	bool show_detect_window = false;		//! ��ʾ����
	bool category_filter = false;			//! ���ɸѡ
	bool first_class = false;				//! ��һ�����
	bool second_class = false;				//! �ڶ������
	bool third_class = false;				//! ���������
	bool fourth_class = false;				//! ���ĸ����
	bool auto_trigger = false;				//! �Զ��������
	bool keep_move = false;					//! ��������
};

struct SharedMemory {
	Data s_data;
	Signal s_signal;
	Info s_info;
};
