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
#include "lock_base.h"
#include "IState.h"
#include "yolo_base.h"
#include "control_base.h"

class Functional: public LOCK {
public:
	Functional(LockInfo info);
	//! ��ʼ��lock 
	IStates initLock() override;

	//! ��ʼ����
	void action() override;
	//! �ͷ�
	void Release() override;
private:

	//! �����׼�ǵľ���
	void categoryFilter(std::vector<float>* distance, std::vector<int>* indices, int idx);
	//! ִ�а��
	void executeTrigger(TargetInfo* target);
	//! �Զ����
	void autoTrigger(TargetInfo* target);
	//! ���ƶ�
	void onlyMcove(TargetInfo* target);
	~Functional();
};

