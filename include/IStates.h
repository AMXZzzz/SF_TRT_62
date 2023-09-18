#pragma once
#include <string>

struct IStates {
	bool is_success() {
		return code == true;
	}	
	
	bool is_error() {
		return code != true;
	}

	std::string getErrorInfo() {
		return info;
	}

	IStates() {};
	IStates(bool err, std::string msg) : code(err), info(msg) {}

private:
	bool code = true;;
	std::string info = "None";

};
