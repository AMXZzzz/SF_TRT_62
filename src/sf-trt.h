#pragma once
#include <string>
#include <spdlog/sinks/basic_file_sink.h>
#include <opencv2/opencv.hpp>

#define WINDOWS_NAME "Smart Monitor"
#define MESSAGEBOX_TITLE "Monitor"
#define MESSAGEBOXA(x,y) MessageBoxA(NULL, x, "SF_62", y)	// 0x00000000L is MB_OK
#define asserthr(x) if(!x) return FALSE;
#define tostr(x) std::to_string(x).c_str()

struct Process {
	std::vector<cv::Rect> _boxes;
	std::vector<int> _indices;
	std::vector<int> _classes;
	std::vector<float> _confidences;
};



enum State:int {
	UNKONEERR = -1,				// 未定义错误

	Success = 0,

	DimsNotEqual = 1,
	FrameNotMatch = 2,

	TRT_Runtime = 3,
	TRT_Engien = 4,
	TRT_Context = 5,
	TRT_LoadFlie = 6,
	TRT_LoadDate = 7,

	CONF_EMPTY = 8,
	IOU_EMPTY = 8,
	ShowWindow_EMPTY = 8,
	PRE_EMPTY = 8,

};

struct IStates {
	//! 默认构造的对象是成功
	IStates() {
		_code = State::Success;
		_msg = "None";
	};
	IStates(State code, std::string msg) :_code(code), _msg(msg) {}
	virtual ~IStates() {};

	virtual bool is_success() {
		return _code == State::Success;
	}
	virtual bool is_error() {
		return _code != State::Success;
	}
	virtual std::string msg() {
		return _msg;
	}
	virtual State err_code() {
		return _code;
	}

protected:
	State _code = State::Success;
	std::string _msg = "None";
};

namespace sf {
	namespace Type {
		enum YoloType :int {
			TYPE_UNKONE = 0,
			TYPE_YOLOV5 = 1,
			TYPE_YOLOV8 = 2,
			TYPE_YOLOX = 3,

		};
		enum FrameType :int {
			DML_FRAME = 1,
			TRT_FRAME = 2,
		};

		enum MoveWay :int {
			GHUB = 0,
			SendInput = 1,
			KMBOX_B = 2
		};

	}
}
