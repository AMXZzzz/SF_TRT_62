#include <yolo.h>

#define MAX_SIZE 256
#define MapFileName L"SF_TRT_62"

struct Parameter {
	//! ��Ҫ����Ĳ���
	float conf = 0.3;						//�� ���Ŷ�ָ��
	float iou = 0.1;						//�� iouָ��
	int equipment = 0;						//�� �豸
	int yolo_type = 0;						//�� yolo����
	int backend = 0;						//�� �������
	std::string module_path;					//�� ģ��·��
	//std::shared_ptr<spdlog::logger> g_logger;	//�� ��־����ָ��

	//! �ź���
	bool uiSendStop = false;		//�� ui����ֹͣ�źţ� true Ϊ������ֹͣ�ź�
	bool aiStatus = false;			//�� ai����״ָ̬��, true Ϊ�����У�false Ϊֹͣ
	bool showWindows = false;		//�� �Ƿ���ʾ����,true Ϊ��ʾ��false Ϊ����ʾ
};
