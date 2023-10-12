#pragma once
#include <string>
#include "parameter.h"
#include "yolo.h"

#define MAX_SIZE 256
#define MapFileName L"SF_TRT_62"

typedef bool(*CustomizeConitor)(int);

struct IParameter {
	//! ��Ҫ����Ĳ���
	float conf = 0.3;						//�� ���Ŷ�ָ��
	float iou = 0.1;						//�� iouָ��
	int equipment = 0;						//�� �豸
	int yolo_type = 0;						//�� yolo����
	int backend = 0;						//�� �������
	int lock_method = 0;					//!  �ƶ��߼���ʽ�� 0�� ����ʽ��1�� ���߳�
	std::string module_path;				//�� ģ��·��
	CustomizeConitor conitor;				//�� �Զ����������		

	//! ���ڿ��ƵĲ���
	bool pid_switch = false;		//�� pid���أ�true Ϊ����false Ϊ��
	float P_x = 0.3f;				//�� x��PID����
	float I_x = 0.1f;
	float D_x = 0.1f;
	float P_y = 0.3f;				//�� y��PID����
	float I_y = 0.1f;
	float D_y = 0.1f;

	//! fov
	bool fov_switch = false;		//�� fov���أ�true Ϊ����false Ϊ��
	float vfov = 0.1f;				//�� ʵ��vfov����
	float hfov = 0.1f;				//!  ʵ��hfov����
	int game_x_pixel = 0;			//�� ��Ϸx������	
	int game_y_pixel = 0;			//�� ��Ϸy������

	//�� �������
	int max_pixels = 100;

	//�� �Զ����
	bool auto_fire = false;			//�� �Զ�������أ�true Ϊ����false Ϊ��
	int auto_random = 0;			//�� �Զ���������
	int auto_interval = 0;			//�� �Զ�������
	int auto_model = 0;				//!  ���ģʽ
	float trigger_w = 0.75f;		//!  ���������Χ
	float trigger_h = 0.75f;		//!  ���������Χ
	unsigned char auto_key = 0x01;			//!  �����������

	//! ���鷽ʽ
	int lock_model = 0;				//�� ���鷽ʽ
	float location = 0.f;			//�� ��׼λ��
	int scops = 0;					//�� ��׼��Χ

	//! ����
	bool lock_switch = false;		//�� ���鿪�أ�true Ϊ����false Ϊ��
	int lock_key = 0;				//�� ���鰴��
	int lock_key2 = 0;				//�� ���鰴��2
	int monitor_module = 0;			//�� ������ʽ  0��API���� 1��Ӳ������

	//! �ź���
	bool uiSendStop = false;		//�� ui����ֹͣ�źţ� true Ϊ������ֹͣ�ź�
	bool aiStatus = false;			//�� ai����״ָ̬��, true Ϊ�����У�false Ϊֹͣ
	bool showWindows = false;		//�� �Ƿ���ʾ����,true Ϊ��ʾ��false Ϊ����ʾ
	bool chooseSwitch = false;		//�� ���ѡ�񿪹أ�true Ϊ����false Ϊ��
	bool class0 = false;			//�� ���0ѡ�񿪹أ�true Ϊ����false Ϊ��
	bool class1 = false;			//�� ���1ѡ�񿪹أ�true Ϊ����false Ϊ��
	bool class2 = false;			//�� ���2ѡ�񿪹أ�true Ϊ����false Ϊ��
	bool class3 = false;			//�� ���3ѡ�񿪹أ�true Ϊ����false Ϊ��

};