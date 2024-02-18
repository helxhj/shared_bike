#include "globals.h"
typedef struct _ConnectStat  ConnectStat;

#define BUFLEN  1024

struct _ConnectStat {
	int fd;
	char send_buf[BUFLEN];
	// 添加一个参数，用来保存事件
	struct event* ev;
	PF *handler;//不同页面的处理函数
};

// 事件集
struct event_base* ev_base;

//echo 服务实现相关代码
ConnectStat * stat_init(int fd);
// void accept_connection(int fd, void *data);
void accept_connection(int fd, short events,void *data);

// void do_welcome_handler(int fd, void  *data);
void do_welcome_handler(int fd, short events,void *data);

// void do_echo_handler(int fd, void  *data);
void do_echo_handler(int fd, short events,void *data);
void do_echo_response(int fd,void *data);
//void do_echo_timeout(int fd, void *data);



void usage(const char* argv)
{
	printf("%s:[ip][port]\n", argv);
}

void set_nonblock(int fd)
{
	int fl = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

 //创建一个套接字，绑定，检测服务器
int startup(char* _ip, int _port) 
{
	//sock
	//1.创建套接字
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("sock");
		exit(2);
	}

	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	//2.填充本地 sockaddr_in 结构体（设置本地的IP地址和端口）
	struct sockaddr_in local;
	local.sin_port = htons(_port);
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr(_ip);

	//3.bind（）绑定
	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
	{
		perror("bind");
		exit(3);
	}
	//4.listen（）监听 检测服务器
	if (listen(sock, 5) < 0)
	{
		perror("listen");
		exit(4);
	}
	
	return sock;    //这样的套接字返回
}


ConnectStat * stat_init(int fd) {
	ConnectStat * temp = NULL;
	temp = (ConnectStat *)malloc(sizeof(ConnectStat));

	if (!temp) {
		fprintf(stderr, "malloc failed. reason: %m\n");
		return NULL;
	}

	memset(temp, '\0', sizeof(ConnectStat));
	temp->fd = fd;
	//temp->status = 0;
}

void do_welcome_handler(int fd, short events,void *data) {
	const char * WELCOME= "Welcome.\n";
	int wlen = strlen(WELCOME);
	int n ;
	ConnectStat * stat = (ConnectStat *)(data);
	
	if( (n = write(fd, "Welcome.\n",wlen)) != wlen ){
		
		if(n<=0){
		    fprintf(stderr, "write failed[len:%d], reason: %s\n",n,strerror(errno));
		}else fprintf(stderr, "send %d bytes only ,need to send %d bytes.\n",n,wlen);
		
	}else {
		// commUpdateReadHandler(fd, do_echo_handler,(void *)stat);
		//commSetTimeout(fd, 10, do_echo_timeout, (void *)stat);
		
		// event_set ，重新设置事件，第一个参数是原来的创建的事件
		event_set(stat->ev,fd,EV_READ,do_echo_handler,(void*)stat);
		// stat->ev中有一个参数是ev_base(事件集合)，这时事件集合是被清空的，需要重新指定
		stat->ev->ev_base = ev_base;
		// 事件添加
		event_add(stat->ev,NULL);
	}
}


// void do_echo_handler(int fd, void  *data) {
void do_echo_handler(int fd, short events,void *data){
	ConnectStat * stat = (ConnectStat *)(data);
	char * p = NULL;
	
	assert(stat!=NULL);
	
	
	p = stat->send_buf;
	*p++ = '-';
	*p++ = '>';
	// p-stat->send_buf 就是已经在数组中写入的字节数
	// p指针往后偏移了2字节，		stat->send_buf指向的是起始位置
	// p -   stat->send_buf  其实就是p偏移的字节数   (char 类型  1字节)
	ssize_t _s = read(fd, p, BUFLEN-(p-stat->send_buf)-1); //2字节"->" +字符结束符.
    if (_s > 0)
    {
		
		*(p+_s) = '\0';
		printf("receive from client: %s\n", p);
		//_s--;
		//while( _s>=0 && ( stat->send_buf[_s]=='\r' || stat->send_buf[_s]=='\n' ) ) stat->send_buf[_s]='\0';
		
		if(!strncasecmp(p, "quit", 4)){//退出.
			comm_close(fd);
                        free(stat);
			return ;
		}
		//write(fd,
		commUpdateWriteHandler(fd, do_echo_response, (void *)stat);
		//commSetTimeout(fd, 10, do_echo_timeout, (void *)stat);
	}else if (_s == 0)  //client:close
    {
        fprintf(stderr,"Remote connection[fd: %d] has been closed\n", fd);
        comm_close(fd);
        free(stat);
    }
    else //err occurred.
    {
        fprintf(stderr,"read faield[fd: %d], reason:%s [%d]\n",fd , strerror(errno), _s);
    }
}

void do_echo_response(int fd, void  *data) {
	ConnectStat * stat = (ConnectStat *)(data);
	int len = strlen(stat->send_buf);
	int _s = write(fd, stat->send_buf, len);
	
	if(_s>0){
		//commSetTimeout(fd, 10, do_echo_timeout, (void *)stat);
		commUpdateReadHandler(fd, do_echo_handler, (void *)stat);
		
	}else if(_s==0){
		fprintf(stderr,"Remote connection[fd: %d] has been closed\n", fd);
                comm_close(fd);
                free(stat);
	}else {
		fprintf(stderr,"read faield[fd: %d], reason:%s [%d]\n",fd ,_s ,strerror(errno));
	}
}

//read()
//注册写事件
  
//写事件就绪
//write()

// void accept_connection(int fd, void *data){
void accept_connection(int fd, short events,void *data){
	struct sockaddr_in peer;
	socklen_t len = sizeof(peer);

	ConnectStat * stat = (ConnectStat *)data;
	
	int new_fd = accept(fd, (struct sockaddr*)&peer, &len);

	if (new_fd > 0)
	{
		ConnectStat *stat = stat_init(new_fd);
		set_nonblock(new_fd);

		printf("new client: %s:%d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
		// commUpdateWriteHandler(new_fd, do_welcome_handler, (void *)stat);
		//commSetTimeout(new_fd, 30,do_echo_timeout, (void *)stat);
		
		// 创建事件
		struct event* ev = event_new(ev_base,new_fd,EV_WRITE,do_welcome_handler,(void*)stat);
		stat->ev = ev;
		// 添加到事件集中
		event_add(ev,NULL);
		
	}
}

/**void do_echo_timeout(int fd, void *data){
	fprintf(stdout,"---------timeout[fd:%d]----------\n",fd);
	comm_close(fd);
        free(data);
}*/



int main(int argc,char **argv){

	if (argc != 3)     //检测参数个数是否正确
	{
		usage(argv[0]);
		exit(1);
	}

	int listen_sock = startup(argv[1], atoi(argv[2]));      //创建一个绑定了本地 ip 和端口号的套接字描述符
	//初始化异步事件处理框架epoll
	
	// comm_init(102400);
	
	// 1、创建事件集
	ev_base = event_base_new();
	
	// 2、创建事件
	
	// 事件集，监听的文件句柄，事件，回调函数，参数
	struct event* ev = event_new(ev_base,listen_sock,EV_READ|EV_PERSIST,accept_connection,NULL);
	// 3、将事件添加到事件集中
	event_add(ev,NULL);
	
	// ConnectStat * stat = stat_init(listen_sock);
	// commUpdateReadHandler(listen_sock,accept_connection,(void *)stat);

	do{
		//不断循环处理事件
		comm_select(1000);
		
	}while(1==1);

	comm_select_shutdown();



}




