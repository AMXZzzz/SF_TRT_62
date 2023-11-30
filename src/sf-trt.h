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

//! ���������ָ��
struct Data {
	float conf = 0.3;
	float iou = 0.1;
	std::shared_ptr<spdlog::logger> m_logger;  //! ��־ָ��

};

//! ������ź�
struct Signal {
	bool shared_memory_succecc = false;		//! �����ڴ��Ƿ��ʼ�����
	bool dll_exit_signal = false;			//! ����Ϊtrueʱ������dll�˳��ź�
	bool ai_start_signal = false;			//! ��Ϊtrueʱ������ai�߳�
	bool show_detect_window = false;				//! ��ʾ����
};

//! �����������Ϣ
struct Info {
	int yolo_tyoe = 0;						//! yolo����
	int frame_type = 0;						//! �������
	int equipment = 0;						//! �豸����
};

struct SharedMemory {
	Data s_data;
	Signal s_signal;
	Info s_info;
};
