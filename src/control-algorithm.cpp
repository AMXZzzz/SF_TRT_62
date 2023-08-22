#include "control-algorithm.h"
#include <cmath>
#include <iostream>


VOID Algorithm::PidControl(float* input, float kp, float ki, float kd) {
	deviation = *input - target_amount;
	P = deviation * kp;
	I = (I + deviation) * ki;
	D = (deviation - last_deviation) * kd;
	*input = P + I + D;
	last_deviation = deviation;
}


VOID Algorithm::HFOVControl(float* input, const int& pixel, const int& worh) {
	float x = (worh * 0.5f) / tanf(106.260205f * pi / 180 * 0.5f);
	*input = atanf(*input * (pixel / (360.0f * pi / 180.0f) * (1.33f / 1.33f)) / x);
}

VOID Algorithm::VFOVControl(float* input, const int& pixel, const int& worh) {
	float y = (worh * 0.5f) / tanf(73.739795f * pi / 180 * 0.5f);
	*input = atanf(*input * (pixel / (180.0f * pi / 180.0f) * (1.33f / 1.33f)) / y);
}


VOID Algorithm::MaxMovePixel(float* input, const int& max) {
	if (max != 0 && abs(*input) > max) {
		if (*input > 0)
			*input = (max);
		if (*input < 0)
			*input = (-max);
	}
}

VOID Algorithm::MinMovePixel(float* input, const int& min) {
	if (min != 0 && abs(*input) < min) {
		*input = 0;
	}
}
