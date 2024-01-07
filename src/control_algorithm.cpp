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
#include <cmath>
#include "control_algorithm.h"


float Control::PidControl(const float input, float kp, float ki, float kd) {
	deviation = input - target_amount;
	P = deviation * kp;
	I = (I + deviation) * ki;
	D = (deviation - last_deviation) * kd;
	//*input = P + I + D;				// ����ĵõ������룬ָ���޸�
	last_deviation = deviation;
	return P + I + D;
}

float Control::FOVControl(const float input, const float& fov, const int& pixel, const int& worh, const int& degree) {
	float out = (worh * 0.5f) / (tan(fov * pi / 180 * 0.5f));
	return atan(input / out) * (pixel / (degree * pi / 180));
}

float Control::MaxMovePixel(const float input, const int max) {
	if (abs(input) > max) {
		if (input > 0)
			return (max);
		if (input < 0)			// ���ܰ���0�������ƶ�ֵ����
			return (-max);
	}
	//! δ���з���ԭֵ
	return input;
}
