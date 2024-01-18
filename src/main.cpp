#include <stdio.h>
#include "iniconfig.h"
#include "configdef.h" // 至于为什么没有写 "common/configdef.h" ，是因为这个用
// 写入make文件

// 如  ./a.out   ./conf/shared_bike.ini
int main(int argc,char** argv){ // 配置文件直接通过参数进行传递
	if(argc != 2){
		printf("程序启动传递参数不正确，请重新启动.\n");
		return -1;
	}
	
	Iniconfig ini; // 解析文件
	if(!ini.loadfile(argv[1])) // 加载并解析配置文件
	{
		fprintf(stderr,"加载配置文件失败，加载文件:%s\n",argv[1]);
		return -2;
	}
	
	// 获取配置文件的结构体
	_st_env_config config = ini.getconfig();
	// 打印一下结构体中的数据
	printf("[database]:\nip\t: %s \nport\t: %d\nuser\t: %s\npwd\t: %s\ndb\t: %s\n[server]:\nsvr_port\t: %d\n",
			config.db_ip.c_str() , config.db_port , config.db_user.c_str() , config.db_pwd.c_str() , 
			config.db_name.c_str() , config.svr_port);
	
	return 0;
}