#ifndef _SHBK_COMMON_INIFONFIG
#define _SHBK_COMMON_INIFONFIG

// 解析ini文件
#include <string>
#include "configdef.h"
class Iniconfig
{
public:
	Iniconfig();
	~Iniconfig();
	
	// 加载配置ini 文件
	bool loadfile(const std::string& path);
	// 获取配置文件，设计一个结构体，直接返回结构体
	const st_env_config& getconfig();
	
private:
	st_env_config _config;
	bool _isloaded;	// 是否已经加载，不让其重复加载
};

#endif

