/*
* 免责声明：
* 本软件是开源程序，您可以自由使用、修改和发布它，使用 GNU 通用公共许可证版本 2（GPLv2）的条款。
* 请注意，在根据GPLv2发布本软件之前，您必须遵守以下条款：
* 1. 任何修改和派生作品必须同样采用GPLv2，并在您的派生作品中提供GPLv2的完整文本。
* 2. 您必须保留原始代码的版权通告，并在您的项目中明确指明原始作者和软件的来源。
* 3. 如果您分发本软件，您必须提供完整的源代码，以满足GPLv2的要求。这包括您的修改和派生作品的源代码。
* 4. 本软件是按"原样"提供的，不附带任何明示或暗示的保证。作者对于使用本软件造成的任何直接、间接、偶发、特殊、典型或惩戒性损害不承担责任。
* 使用此代码即表示您同意遵守GPLv2的所有要求和免责声明的条件。
* 请仔细阅读GNU通用公共许可证版本2以了解更多详细信息。您可以在 http ://www.gnu.org/licenses/gpl-2.0.html 上找到GPLv2的完整文本。
* [作者姓名]: bilibili：随风而息
* [地址]:https://github.com/AMXZzzz/SF_TRT_62.git
* [日期]: 2023/10/26
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
	std::cout << "[debug]: PID 构造" << std::endl;
}

PID::~PID() {
	std::cout << "[debug]: PID 析构" << std::endl;
}