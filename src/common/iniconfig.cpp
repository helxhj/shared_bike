#include "iniconfig.h"
#include <iniparser/iniparser.h>

Iniconfig::Iniconfig() : _isloaded(false){
	
}

Iniconfig::~Iniconfig(){
}

// 加载配置ini 文件
bool Iniconfig::loadfile(const std::string& path){
	// 可以判断文件是否已经被加载了
	//if(_isloaded)... 怎么样
	// 文件已经被加载了 就不让其在进行加载
	
	dictionary* ini;		// 定义一个字典，通过字典获取配置文件中的数据
	ini = iniparser_load(path.c_str()); 
	if(ini == NULL){ // 加载失败，可能是文件不存在
		fprintf(stderr, "cannot parse file: %s\n", path.c_str());
        return -1 ;
	}
	
	// 获取数据
	const char* ip = iniparser_getstring(ini,"database:ip","127.0.0.1");
	unsigned short port = iniparser_getint(ini,"database:port",3306);
	const char* user = iniparser_getstring(ini,"database:user","root");
	const char* pwd = iniparser_getstring(ini,"database:pwd","123456");
	const char* db = iniparser_getstring(ini,"database:db","qiniubike");
	unsigned short svr_port = iniparser_getint(ini,"server:port",9090);
	
	// 进行赋值		
	_config = st_env_config(std::string(ip) , port , std::string(user) , std::string(pwd) , std::string(db),
									svr_port);
	
	// 将字典指针释放掉
	iniparser_freedict(ini);
	_isloaded = true; // 加载ini文件标志
	return true;
}

// 获取配置文件
const st_env_config& Iniconfig::getconfig(){
	return _config;
}










