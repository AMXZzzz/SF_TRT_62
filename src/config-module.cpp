#include "config-module.h"
#include <math.h>
#include <fstream>
#include <iostream>

static char* Node = "Imgui";

#pragma region 小组件 - 代码区

static inline VOID GetConfigPath(const char* config_name, char** config_path) {
	char path[MAX_PATH]{};
	GetModuleFileNameA(NULL, path, MAX_PATH);
	static std::string root_path = path;
	root_path = root_path.substr(0, root_path.find_last_of("\\") + 1) + config_name;
	*config_path = (char*)root_path.c_str();
}

static inline VOID CreateConfigFile(const char* file_path) {
	std::ifstream file(file_path);
	if (!file.good()) {
		std::ofstream p(file_path, std::ios_base::out);
		p.close();
	}
}

static inline BOOL CheckConfigFile(const char* file_path) {
	std::ifstream file(file_path);
	return file.good();
}

#pragma endregion 

#pragma region 写入Config - 代码区

static inline VOID WriteConfig(const char* key, char* val, char* path) {
	WritePrivateProfileStringA(Node, key, val, path);
}
static inline VOID WriteConfig(const char* key, float val, char* path) {
	WritePrivateProfileStringA(Node, key, std::to_string(val).c_str(), path);
}
static inline VOID WriteConfig(const char* key, int val, char* path) {
	WritePrivateProfileStringA(Node, key, std::to_string(val).c_str(), path);
}

#pragma endregion 

#pragma region 读取Config - 代码区

static inline VOID LoadParame(char* val, char(**parame)[MAX_PATH], char* config_path) {
	char temp[MAX_PATH]{};
	GetPrivateProfileStringA(Node, val, NULL, temp, MAX_PATH, config_path);
	strcpy_s(**parame, temp);							// **parame 为utf8， cmd乱码,不影响使用
	//strcpy(**parame, UTF8ToAnsi(temp).c_str());	// **parame 为Ansi ,imgui乱码 ，影响观感
}

static inline VOID LoadParame(char* val, bool* parame, char* config_path) {
	*parame = GetPrivateProfileIntA(Node, val, NULL, config_path);
}

static inline VOID LoadParame(char* val, int* parame, char* config_path) {
	*parame = GetPrivateProfileIntA(Node, val, NULL, config_path);
}
static inline VOID LoadParame(char* val, float* parame, char* config_path) {
	char str_temp[MAX_PATH]{};
	GetPrivateProfileStringA(Node, val, NULL, str_temp, MAX_PATH, config_path);
	*parame = std::strtof(str_temp, NULL);
}
#pragma endregion 

#pragma region 操作Config - 代码区

static VOID ReadAndWrite(char* key, char(*val)[MAX_PATH], char* path, CONFIG_IO row) {
	if (row == CONFIG_READ)
		LoadParame(key, &val, path);
	else
		WriteConfig(key, *val, path);
}

static VOID ReadAndWrite(char* key, bool* val, char* path, CONFIG_IO row) {
	if (row == CONFIG_READ)
		LoadParame(key, val, path);
	else
		WriteConfig(key, *val, path);
}

static VOID ReadAndWrite(char* key, int* val, char* path, CONFIG_IO row) {
	if (row == CONFIG_READ)
		LoadParame(key, val, path);
	else
		WriteConfig(key, *val, path);
}

static VOID ReadAndWrite(char* key, float* val, char* path, CONFIG_IO row) {
	if (row == CONFIG_READ)
		LoadParame(key, val, path);
	else
		WriteConfig(key, *val, path);
}
#pragma endregion 

VOID OperationParame(CONFIG_IO row, char* path , Parameter* parame) {
	
	ReadAndWrite("yolo_type", &parame->yolo_type, path, row);
	ReadAndWrite("model_path", &parame->model_path, path, row);
	ReadAndWrite("backend", &parame->backend, path, row);
	ReadAndWrite("move_way", &parame->move_way, path, row);
	ReadAndWrite("ShowWindow", &parame->showWindow, path, row);

	ReadAndWrite("key_ui", &parame->key_ui, path, row);
	ReadAndWrite("lock_key", &parame->lock_key, path, row);
	ReadAndWrite("lock_key2", &parame->lock_key2, path, row);
	ReadAndWrite("auto_fire", &parame->auto_fire, path, row);
	ReadAndWrite("auto_interval", &parame->auto_interval, path, row);
	ReadAndWrite("auto_random", &parame->auto_random, path, row);
	ReadAndWrite("auto_model", &parame->auto_model, path, row);

	ReadAndWrite("parame_ui", &parame->parame_ui, path, row);
	ReadAndWrite("confidence", &parame->iou, path, row);
	ReadAndWrite("location", &parame->location, path, row);
	ReadAndWrite("effectiverange", &parame->effectiverange, path, row);
	ReadAndWrite("max_range", &parame->max_range, path, row);
	ReadAndWrite("max_pixels", &parame->max_pixels, path, row);
	ReadAndWrite("ModelClasses", &parame->ModelClasses, path, row);
	ReadAndWrite("class0", &parame->class0, path, row);
	ReadAndWrite("class1", &parame->class1, path, row);
	ReadAndWrite("class2", &parame->class2, path, row);
	ReadAndWrite("class3", &parame->class3, path, row);

	ReadAndWrite("pid_ui", &parame->pid_ui, path, row);
	ReadAndWrite("P_x", &parame->P_x, path, row);
	ReadAndWrite("I_x", &parame->I_x, path, row);
	ReadAndWrite("D_x", &parame->D_x, path, row);
	ReadAndWrite("P_y", &parame->P_y, path, row);
	ReadAndWrite("I_y", &parame->I_y, path, row);
	ReadAndWrite("D_y", &parame->D_y, path, row);

	ReadAndWrite("fov_algorithm", &parame->fov_algorithm, path, row);
	ReadAndWrite("game_x_pixel", &parame->game_x_pixel, path, row);
	ReadAndWrite("game_y_pixel", &parame->game_y_pixel, path, row);

}

VOID ConfigModule::GetParame(char* path, Parameter* parame) {
	if (!CheckConfigFile(path)) return;		// 没有config文件，跳过，否者conf会有0出现
	OperationParame(CONFIG_READ,path , parame);
}

VOID ConfigModule::SaveParame(Parameter* parame) {
	char* path;
	GetConfigPath(Config_name, &path);
	CreateConfigFile(path);
	OperationParame(CONFIG_WRITE, path , parame);
}