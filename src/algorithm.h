#pragma once

class Algorithm {
public:
	void PidControl(float*, float, float, float);
	void FOVControl(float*, const float&, const int&, const int&, const int&);
	void MaxMovePixel(float*, const int&);
private:
	float deviation = 0;		// 偏差量
	float last_deviation = 0;		// 上一次的偏差量
	float target_amount = 0;	// 目标量，恒定为0
	float P = 0;
	float I = 0;
	float D = 0;
	float pi = 3.14159265358979323846;
};

