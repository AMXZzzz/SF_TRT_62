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
#include <spdlog/sinks/basic_file_sink.h>
#include <thread>
#include "yolo_base.h"
#include "frame_base.h"
#include "sf-trt.h"

class Actuator {
public:
	//! �����߳�
	void start();
	//! �˳��߳�
	void exit();
	//! �ȴ��߳��˳�
	void join();
	//! �вι���
	Actuator(MemorySignal* signal) : m_signal(signal){
		if (signal == nullptr)  std::cout << "����Actuator �� MemorySignal ָ��Ϊ��" << std::endl;
	}
private:
	std::shared_ptr<spdlog::logger> m_logger;	//����־����ָ��
	MemorySignal* m_signal;						//! �����ڴ��ź�
	YOLO* m_yolo ;								//! yolo���ͻ���
	Frame* m_frame ;							//! �����˿��
	Process* m_process;							//! Ԥ���󣩴�������
	std::thread actuatorThreadHandle;
	bool m_exit_signal = false;
	//! ��ʼ��log
	bool setSpdlog();
	//! ��ʼ��yolo�������
	//! type: yolo���ͣ�0��yolov5/v7 1��yolov8 2��yolox
	bool setYoloType(int type);
	//! ��ʼ��������
	//! type: �����ܣ�0��TensorRt�� 1��DML
	//! equipment: �����豸������Ĭ��0
	bool setFrameBack(int type, int equipment = 0);
	//! �����߳�
	void word();
	//! ����Ĭ�Ϲ���
	Actuator() {}
};

