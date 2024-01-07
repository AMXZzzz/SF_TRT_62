#include "control_coupling-pid.h"

IStates CouplingPID::init() {
    return IStates();
}

float CouplingPID::control_x(const float input, Data data) {
    double error = input - 0;
    v_x.integral += error;

    double derivative = error - v_x.last_error;

    double output = v_x.kp * error + v_x.ki * v_x.integral + v_x.kd * derivative;

    double alpha = v_x.beta / (v_x.beta + abs(error));
    v_x.kp += v_x.gamma * alpha * error;
    v_x.ki += v_x.gamma * alpha * v_x.integral;
    v_x.kd += v_x.gamma * alpha * derivative;

    v_x.last_error = error;

    return output;
}

float CouplingPID::control_y(const float input, Data data) {
    double error = input - 0;
    v_y.integral += error;

    double derivative = error - v_y.last_error;

    double output = v_y.kp * error + v_y.ki * v_y.integral + v_y.kd * derivative;

    double alpha = v_y.beta / (v_y.beta + abs(error));
    v_y.kp += v_y.gamma * alpha * error;
    v_y.ki += v_y.gamma * alpha * v_y.integral;
    v_y.kd += v_y.gamma * alpha * derivative;

    v_y.last_error = error;

    return output;
}

bool CouplingPID::Release() {
    return false;
}