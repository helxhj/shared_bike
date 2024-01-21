#ifndef DISTRIBUTED_LOGGER_H
#define DISTRIBUTED_LOGGER_H

#include <string>
#include <log4cpp/Category.hh>

class Logger{
		// 设计
public:
	bool init(const std::string& log_conf_file); // 读取配置文件
	// 单例
	static Logger* instance(){
		return &instance_;
	}
	
	log4cpp::Category* GetHandle(){ 
		return category_;
	}
	
	
private:
	static Logger instance_; // 全局只有一个
	log4cpp::Category* category_;
};

#define LOG_INFO Logger::instance()->GetHandle()->info
#define LOG_DEBUG Logger::instance()->GetHandle()->debug
#define LOG_ERROR Logger::instance()->GetHandle()->error
#define LOG_WARN Logger::instance()->GetHandle()->warn

#endif