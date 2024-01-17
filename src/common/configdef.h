
#ifndef _SHBK_COMMON_CONFIGDEF
#define _SHBK_COMMON_CONFIGDEF	
// 配置项定义
typedef struct st_env_config
{
		// 数据库的配置
		std::string db_ip;		// 数据库地址
		unsigned short db_port;	// 端口号
		std::string db_user;
		std::string db_pwd;
		std::string db_name;
		// 服务器的配置
		unsigned short svr_port; // 服务器端口号
		
		// 默认构造
		st_env_config(){}
		// 有参构造
		st_env_config(std::string db_ip,unsigned short db_port,std::string db_user,
				std::string db_pwd,std::string db_name,unsigned short svr_port){			
			this->db_ip = db_ip;		// 数据库地址
			this->db_port = db_port;	// 端口号
			this->db_user = db_user;
			this->db_pwd = db_pwd;
			this->db_name = db_name;
			this->svr_port = svr_port; 
		}
		
		// 拷贝构造
		st_env_config& operator=(const st_env_config& config){
			if(this != &config){
				this->db_ip = config.db_ip;		// 数据库地址
				this->db_port = config.db_port;	// 端口号
				this->db_user = config.db_user;
				this->db_pwd = config.db_pwd;
				this->db_name = config.db_name;
				this->svr_port = config.svr_port; 
			}
			return *this;
		}	
}_st_env_config;

#endif