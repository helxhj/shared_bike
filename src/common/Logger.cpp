#include "Logger.h"
#include <iostream>
#include <log4cpp/RemoteSyslogAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PropertyConfigurator.hh>

// 静态成员变量，类内定义 ，类外初始化
Logger Logger::instance_;

// 读取配置文件
bool Logger::init(const std::string& log_conf_file){
	try
    {
        log4cpp::PropertyConfigurator::configure(log_conf_file);
    }
    catch(log4cpp::ConfigureFailure& f)
    {
        std::cerr << " load log config file " << log_conf_file.c_str() << " failed with result : " << f.what()<< std::endl;
        return false;
    }

    category_ = &log4cpp::Category::getRoot();

    return true;

}


