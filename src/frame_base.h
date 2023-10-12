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
#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include "yolo_base.h"
#include "IState.h"


//! 框架类型命名空间
namespace sf {
	namespace Type {
		enum FrameType :int {
			UBNKONE_FRAME = 0,
			TRT_FRAME = 1,
			DML_FRAME = 2,
		};
	}
}

class Frame {
public:
	Frame(YOLO* yolo, const std::shared_ptr<spdlog::logger>& logger, int equipment)
		: m_yolo(yolo), m_logger(logger), m_equipment(equipment) {
	}

	//! 解析模型
	virtual bool AnalyticalModel(const char* engine_path) = 0;

	//! 解析模型
	virtual bool AnalyticalModel(const std::string engine_path) = 0;

	//! 推理接口
	virtual void Detect(cv::Mat&) = 0;

	//! 释放接口
	virtual void Release() = 0;

	//! 获取错误信息
	virtual	IStates getLastErrorInfo() = 0;

	//! 虚基类析构
	virtual ~Frame() { std::cout << "Frame基类释放" << std::endl; };
protected:
	YOLO* m_yolo;
	std::shared_ptr<spdlog::logger> m_logger = nullptr;
	int m_equipment = 0;
};

//! 工厂函数，用于创建对象
//! 需要填充该结构体
struct FRAMEINFO {
	YOLO* yolo;								//! yolo配置指针
	sf::Type::FrameType frame_type;			//! 框架类型
	std::shared_ptr<spdlog::logger> logger;	//! spd日志指针
	int equipment;							//! 运行设备索引
};

namespace sf {
	Frame* createFrame(FRAMEINFO* info);
}