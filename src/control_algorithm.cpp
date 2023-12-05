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
#include <cmath>
#include "control_algorithm.h"


float Control::PidControl(const float input, float kp, float ki, float kd) {
	deviation = input - target_amount;
	P = deviation * kp;
	I = (I + deviation) * ki;
	D = (deviation - last_deviation) * kd;
	//*input = P + I + D;				// 计算的得到的输入，指针修改
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
		if (input < 0)			// 不能包含0，否者移动值错误
			return (-max);
	}
	//! 未命中返回原值
	return input;
}
