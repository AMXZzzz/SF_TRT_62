#pragma once

class Algorithm {
public:
	void PidControl(float*, float, float, float);
	void FOVControl(float*, const float&, const int&, const int&, const int&);
	void MaxMovePixel(float*, const int&);
private:
	float deviation = 0;		// ƫ����
	float last_deviation = 0;		// ��һ�ε�ƫ����
	float target_amount = 0;	// Ŀ�������㶨Ϊ0
	float P = 0;
	float I = 0;
	float D = 0;
	float pi = 3.14159265358979323846;
};

