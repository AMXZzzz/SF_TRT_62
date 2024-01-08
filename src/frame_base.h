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
#include "yolo_base.h"
#include "IState.h"

//! ������������ռ�
namespace sf {
	namespace Type {
		enum FrameType :int {
			UBNKONE_FRAME = 0,
			TRT_FRAME = 1,
			DML_FRAME = 2,
		};
	}
}

class Frame {
public:
	Frame(YOLO* yolo, const std::shared_ptr<spdlog::logger>& logger, int equipment);

	//! ����ģ��
	virtual bool AnalyticalModel(const char* engine_path) = 0;

	//! ����ģ��
	virtual bool AnalyticalModel(const std::string engine_path) = 0;

	//! ����ӿ�
	virtual void Detect(cv::Mat&) = 0;

	//! �ͷŽӿ�
	virtual void Release() = 0;

	//! ��ȡ������Ϣ
	virtual	IStates getLastErrorInfo() = 0;

	//! ���������
	virtual ~Frame();

protected:
	YOLO* m_yolo;
	std::shared_ptr<spdlog::logger> m_logger = nullptr;
	int m_equipment = 0;

private:
	Frame();
};

//! �������������ڴ�������
//! ��Ҫ���ýṹ��
struct FRAMEINFO {
	YOLO* yolo;								//! yolo����ָ��
	sf::Type::FrameType frame_type;			//! �������
	std::shared_ptr<spdlog::logger> logger;	//! spd��־ָ��
	int equipment;							//! �����豸����
};

namespace sf {
	Frame* createFrameObject(FRAMEINFO* info);
}