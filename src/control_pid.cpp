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
#include "control_pid.h"

IStates PID::init() {
    return IStates();
}

float PID::control_x(const float input, Data data) {
	pid_x.deviation = input - pid_x.target_amount;
	pid_x.P = pid_x.deviation * data.kp_x;
	pid_x.I = (pid_x.I + pid_x.deviation) * data.ki_x;
	pid_x.D = (pid_x.deviation - pid_x.last_deviation) * data.kd_x;
	pid_x.last_deviation = pid_x.deviation;
	return pid_x.P + pid_x.I + pid_x.D;
}

float PID::control_y(const float input, Data data) {
	pid_y.deviation = input - pid_y.target_amount;
	pid_y.P = pid_y.deviation * data.kp_y;
	pid_y.I = (pid_y.I + pid_y.deviation) * data.ki_y;
	pid_y.D = (pid_y.deviation - pid_y.last_deviation) * data.kd_y;
	pid_y.last_deviation = pid_y.deviation;
	return pid_y.P + pid_y.I + pid_y.D;
}

void PID::Release() {
	delete this;
}

//! -------------- [debug] ------------ //
PID::PID() {
	std::cout << "[debug]: PID ����" << std::endl;
}

PID::~PID() {
	std::cout << "[debug]: PID ����" << std::endl;
}