#pragma once
#include "factory-base.h"

struct Parameter {

	//! yolo����
	int yolo_type = 0;
	//! ���
	int backend = 0;
	//! ģ��·��
	char model_path[MAX_PATH] = "";

	float conf = 0.3;
	float iou = 0.1;
	Process process;
	bool uiStop = true;
	bool executionStatus = true;
	//�� ��ʾ������
	bool showWindow = false;

	bool ai_is_run = false;

	bool key_ui = true;		     // ����UI
	bool parame_ui = false;		// ��ʾ����UI
	bool pid_ui = false;			// PIDui
	bool fov_algorithm = false;			// fov ui


	bool AimSwitch = true;			// ����״̬
	int move_way = 0;    //�ƶ���ʽ
	bool ModelClasses = false;		// ���ѡ��

	float location = 0.75;				// ƫ��
	int effectiverange = 200;			// ������Ч��Χ
	int max_pixels = 15;				// �����ƶ�����
	int class_number = 1;				// �������
	int max_range = 640;			// ���Χ

	bool class0 = false;
	bool class1 = false;
	bool class2 = false;
	bool class3 = false;

	float P_x = 0.3;
	float I_x = 0.1;
	float D_x = 0.1;
	float P_y = 0.3;
	float I_y = 0.1;
	float D_y = 0.1;

	int game_x_pixel = 0;		// ��������
	int game_y_pixel = 0;		// ��������

	int lock_key = 0x02;		// ��������
	int lock_key2 = 0;			// �Զ��崥������

	bool auto_fire = false;		// �����Զ�����
	int auto_interval = 150;	// ���150ms
	int auto_random = 80;		// ���ϵ������
	int auto_model = 0;			// ���ģʽ

	//! �ǹ����ڴ�
	bool is_shared() {
		return _shared == true;
	}

	void Release() {
		if (is_shared()) UnmapViewOfFile(this);
		CloseHandle(this->_MapFile);
		delete this;
	}

	void setMapFile(HANDLE MapFile) {
		_MapFile = MapFile;
	}


private:
	bool _shared = false;
	HANDLE _MapFile = NULL;		// ��¼Mapָ��,�������ͷ�

};

class ParameFactory final : public IFactory {
public:
	static ParameFactory* createParameFactory() {
		return new ParameFactory();
	}

	//! ��ȡ����
	IStates QueryInterface(void** parme) override;

	//! ����Ϊ�����ڴ�
	void setSharedMemory();

	//! �ͷ�
	void Release() override;

private:
	HANDLE _MapFile;
	bool is_sharedmemory = false;

	ParameFactory() {}
};
