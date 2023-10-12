#include<cmath>
#include "algorithm.h"

void Algorithm::PidControl(float* input, float kp, float ki, float kd) {
	deviation = *input - target_amount;
	P = deviation * kp;
	I = (I + deviation) * ki;
	D = (deviation - last_deviation) * kd;
	*input = P + I + D;				// ����ĵõ������룬ָ���޸�
	last_deviation = deviation;
	//return out;
}

void Algorithm::FOVControl(float* input, const float& fov, const int& pixel, const int& worh, const int& degree) {
	float out = (worh * 0.5f) / (tan(fov * pi / 180 * 0.5f));
	*input = atan(*input / out) * (pixel / (degree * pi / 180));
}

void Algorithm::MaxMovePixel(float* input, const int& max) {
	if (abs(*input) > max) {
		if (*input > 0)
			*input = (max);
		if (*input < 0)			// ���ܰ���0�������ƶ�ֵ����
			*input = (-max);
	}
}
