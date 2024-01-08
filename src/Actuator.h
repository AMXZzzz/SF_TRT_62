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
#include "dxgi-module.h"
#include "lock_base.h"

class Actuator {
public:

	~Actuator();
	//! �����߳�
	void start();
	//! �˳��߳�
	void exit();
	//! �ȴ��߳��˳�
	void join();
	//! �вι���
	Actuator(SharedMemory* sharedmemory);
	//! �ͷ�
	void Release();
private:
	DXGI* m_dx;									//! dx��ͼ����
	LOCK* m_lock;								//! �����߼�����
	YOLO* m_yolo;								//! yolo����
	Frame* m_frame;								//! �����˶���
	SharedMemory* m_sharedmemory;				//! �����ڴ����
	std::shared_ptr<spdlog::logger> m_logger;	//����־����ָ��

	Process m_process;							//! Ԥ���󣩴�������
	IPoint m_point;								//! �������Ϣ
	bool m_exit_signal = false;					//! �˳��м����
	std::thread actuatorThreadHandle;			//! �߳̾��

	//! ��ʼ����־�����ԣ��Ż���UI��
	bool setSpdlog();
	//! ��ʼ��yolo�������
	bool setYoloConfigObject();
	//! ��ʼ��������
	bool setDetectFrameworkObject();
	//! ��ʼ��dxgi
	bool setDXGICpatureObject();
	//! ����lock����
	bool setLockLogicObject();
	//! ��ʼ����Դ
	bool initializeResources();
	//! �����߳�
	void word();
	Actuator();
};

