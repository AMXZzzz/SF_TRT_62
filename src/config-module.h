#pragma once
#include "parameter.h"

enum CONFIG_IO {
	CONFIG_READ,
	CONFIG_WRITE,
};

struct ConfigModule {
	ConfigModule() {};
	~ConfigModule() {};

	static ConfigModule& Get() {
		static ConfigModule m_pInstance;
		return m_pInstance;
	}

	VOID GetParame(char* path , Parameter* parame);

	VOID SaveParame(Parameter* parame);

	char Config_name[MAX_PATH] = "Config.ini";		// configÎÄ¼þÃû
};
