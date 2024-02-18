#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<assert.h>
#include<fcntl.h>
#include<unistd.h>

//    int fd;
typedef struct _ConnectStat  ConnectStat;

typedef void(*response_handler) (ConnectStat * stat);

struct _ConnectStat {	// 连接状态结构体
	int fd;
	char name[64];
	char  age[64];
	struct epoll_event _ev;
	int  status;//0 -未登录   1 - 已登陆
	response_handler handler;//不同页面的处理函数
};

//http协议相关代码
ConnectStat * stat_init(int fd);
void connect_handle(int new_fd);
void do_http_respone(ConnectStat * stat);
void do_http_request(ConnectStat * stat);
void welcome_response_handler(ConnectStat * stat);
void commit_respone_handler(ConnectStat * stat);


const char *main_header = "HTTP/1.0 200 OK\r\nServer: Martin Server\r\nContent-Type: text/html\r\nConnection: Close\r\n";

static int epfd = 0;

void usage(const char* argv)
{
	printf("%s:[ip][port]\n", argv);
}

void set_nonblock(int fd)
{
	int fl = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

/*******************************************
创建服务器的套接字，绑定http端口(80)  进行监听  ,并设置端口复用
当连接断开时，再次连接就不会出现地址错误
将ip和 端口作为参数传递
如  192.168.85.128    80
****************************************/
int startup(char* _ip, int _port)  //创建一个套接字，绑定，检测服务器
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
	// SO_REUSEADDR 设置地址重用，当服务关闭后，马上在启动一个服务
	// 可能会使用以前的地址，当在bind的时候就会报错
	// 端口复用
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
	//sleep(1000);
	return sock;    //这样的套接字返回
}

int main(int argc, char *argv[])
{
	// ./可执行程序   ip    端口号
	if (argc != 3)     //检测参数个数是否正确
	{
		usage(argv[0]);
		exit(1);
	}

	int listen_sock = startup(argv[1], atoi(argv[2]));      //创建一个绑定了本地 ip 和端口号的套接字描述符


	//1.创建epoll    
	epfd = epoll_create(256);    //可处理的最大句柄数256个，但并不是只能处理256，也可以支持更多链接
	if (epfd < 0)
	{
		perror("epoll_create");
		exit(5);
	}

	struct epoll_event _ev;       //epoll结构填充 
	ConnectStat * stat = stat_init(listen_sock); // 连接状态分配内存并初始化
	_ev.events = EPOLLIN;         //初始关心事件为读
	_ev.data.ptr = stat;
	//_ev.data.fd = listen_sock;    //  

	//2.托管  ，EPOLL_CTL_ADD增加，
	// 向epoll对象中添加事件
	epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &_ev);  //将listen sock添加到epfd中，关心读事件

	// 会将准备好的
	struct epoll_event revs[64];// 同时接收事件数组

	int timeout = -1;  // 无限期监听， 就是阻塞在这
	int num = 0;
	int done = 0;

	while (!done)
	{
		//epoll_wait()相当于在检测事件
		// 当有num数量的事件就绪，就会将事件拷贝到revs数组中
		// 并依次存放，比如num=10，那么事件就会放到revs数组的前10个当中
		// 第一次监听读取时间，就是有数据发来
		switch ((num = epoll_wait(epfd, revs, 64, timeout)))  //返回需要处理的事件数目  64表示 事件有多大
		{
			// 每次写完发送数据，都将托管数据的状态事件设置成POLLIN
			// 每次接收完数据后，需要发送数据，将状态事件设置成POLLOUT
			
		case 0:                  //返回0 ，表示监听超时
			printf("timeout\n"); // 无限期阻塞不会到这个分支
			break;
		case -1:                 //出错
			perror("epoll_wait");
			break;
		default:                 //大于零 即就是返回了需要处理事件的数目
		{
			struct sockaddr_in peer;
			socklen_t len = sizeof(peer);

			int i;
			for (i = 0; i < num; i++)
			{
				// 获取
				ConnectStat * stat = (ConnectStat *)revs[i].data.ptr;

				int rsock = stat->fd; //准确获取哪个事件的描述符
				if (rsock == listen_sock && (revs[i].events) && EPOLLIN) //如果是初始的 就接受，建立链接
				{
					// 表示有新的连接进来了
					int new_fd = accept(listen_sock, (struct sockaddr*)&peer, &len);
	
					if (new_fd > 0)
					{
						printf("get a new client:%s:%d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
						//sleep(1000);
						connect_handle(new_fd);
					}
				}
				else // 接下来对num - 1 个事件处理
				{
					if (revs[i].events & EPOLLIN)
					{
						do_http_request((ConnectStat *)revs[i].data.ptr);
					}
					else if (revs[i].events & EPOLLOUT)
					{
						do_http_respone((ConnectStat *)revs[i].data.ptr);
					}
					else
					{
					}
				}
			}
		}
		break;
		}//end switch
	}//end while
	return 0;
}

/******************************
对连接状态结构体 分配内存并初始化fd 、stat
******************************/
ConnectStat * stat_init(int fd) {
	ConnectStat * temp = NULL;
	temp = (ConnectStat *)malloc(sizeof(ConnectStat));

	if (!temp) { // 分配内存失败
		fprintf(stderr, "malloc failed. reason: %m\n");
		return NULL;
	}

	memset(temp, '\0', sizeof(ConnectStat));
	temp->fd = fd; // 服务器sock
	temp->status = 0;
	//temp->handler = welcome_response_handler;
	return temp;
}

//初始化连接，然后等待浏览器发送请求
void connect_handle(int new_fd) {
	ConnectStat *stat = stat_init(new_fd);
	set_nonblock(new_fd);

	stat->_ev.events = EPOLLIN;
	stat->_ev.data.ptr = stat;

	epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &stat->_ev);    //二次托管

}
// 对http的响应，调用对应函数
void do_http_respone(ConnectStat * stat) {
	stat->handler(stat); // 调用stat中函数指针指向的函数，并将stat作为参数传递
}

void do_http_request(ConnectStat * stat) { // 处理http请求

	//读取和解析http 请求
	char buf[4096];
	char * pos = NULL;
	//while  header \r\n\r\ndata
	ssize_t _s = read(stat->fd, buf, sizeof(buf) - 1);
	if (_s > 0)
	{
		buf[_s] = '\0';
		printf("receive from client:%s\n", buf);

		pos = buf;

		//Demo 仅仅演示效果，不做详细的协议解析
		if (!strncasecmp(pos, "GET", 3)) {
			stat->handler = welcome_response_handler;
		}
		else if (!strncasecmp(pos, "Post", 4)) {
			//获取 uri
			printf("---Post----\n");
			pos += strlen("Post");
			while (*pos == ' ' || *pos == '/') ++pos;

			if (!strncasecmp(pos, "commit", 6)) {//获取名字和年龄
				int len = 0;

				printf("post commit --------\n");
				pos = strstr(buf, "\r\n\r\n");
				char *end = NULL;
				if (end = strstr(pos, "name=")) {
					pos = end + strlen("name=");
					end = pos;
					while (('a' <= *end && *end <= 'z') || ('A' <= *end && *end <= 'Z') || ('0' <= *end && *end <= '9'))	end++;
					len = end - pos;
					if (len > 0) {
						memcpy(stat->name, pos, end - pos);
						stat->name[len] = '\0';
					}
				}

				if (end = strstr(pos, "age=")) {
					pos = end + strlen("age=");
					end = pos;
					while ('0' <= *end && *end <= '9')	end++;
					len = end - pos;
					if (len > 0) {
						memcpy(stat->age, pos, end - pos);
						stat->age[len] = '\0';
					}
				}
				stat->handler = commit_respone_handler;

			}
			else {
				stat->handler = welcome_response_handler;
			}

		}
		else {
			stat->handler = welcome_response_handler;
		}

		//生成处理结果 html ,write

		stat->_ev.events = EPOLLOUT;
		//stat->_ev.data.ptr = stat;  ，第二次修改，修改stat的事件
		epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);    //二次托管
	}
	else if (_s == 0)  //client:close
	{
		printf("client: %d close\n", stat->fd);
		epoll_ctl(epfd, EPOLL_CTL_DEL, stat->fd, NULL);
		close(stat->fd);
		free(stat);
	}
	else
	{
		perror("read");
	}

}


void welcome_response_handler(ConnectStat * stat) {
	const char * welcome_content = "\
<html lang=\"zh-CN\">\n\
<head>\n\
<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\">\n\
<title>This is a test</title>\n\
</head>\n\
<body>\n\
<div align=center height=\"500px\" >\n\
<br/><br/><br/>\n\
<h2>大家好，欢迎来到奇牛学院VIP 课！</h2><br/><br/>\n\
<form action=\"commit\" method=\"post\">\n\
尊姓大名: <input type=\"text\" name=\"name\" />\n\
<br/>芳龄几何: <input type=\"password\" name=\"age\" />\n\
<br/><br/><br/><input type=\"submit\" value=\"提交\" />\n\
<input type=\"reset\" value=\"重置\" />\n\
</form>\n\
</div>\n\
</body>\n\
</html>";

	char sendbuffer[4096];
	char content_len[64];

	strcpy(sendbuffer, main_header);
	snprintf(content_len, 64, "Content-Length: %d\r\n\r\n", (int)strlen(welcome_content));
	strcat(sendbuffer, content_len);
	strcat(sendbuffer, welcome_content);
	printf("send reply to client \n%s", sendbuffer);

	write(stat->fd, sendbuffer, strlen(sendbuffer));
	// 写完后，有设置读取
	stat->_ev.events = EPOLLIN;
	//stat->_ev.data.ptr = stat;  EPOLL_CTL_MOD修改epoll中的事件
	epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);


}


void commit_respone_handler(ConnectStat * stat) {
	const char * commit_content = "\
<html lang=\"zh-CN\">\n\
<head>\n\
<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\">\n\
<title>This is a test</title>\n\
</head>\n\
<body>\n\
<div align=center height=\"500px\" >\n\
<br/><br/><br/>\n\
<h2>欢迎学霸同学&nbsp;%s &nbsp;,你的芳龄是&nbsp;%s！</h2><br/><br/>\n\
</div>\n\
</body>\n\
</html>\n";

	char sendbuffer[4096];
	char content[4096];
	char content_len[64];
	int len = 0;

	len = snprintf(content, 4096, commit_content, stat->name, stat->age);
	strcpy(sendbuffer, main_header);
	snprintf(content_len, 64, "Content-Length: %d\r\n\r\n", len);
	strcat(sendbuffer, content_len);
	strcat(sendbuffer, content);
	printf("send reply to client \n%s", sendbuffer);

	write(stat->fd, sendbuffer, strlen(sendbuffer));

	stat->_ev.events = EPOLLIN;
	//stat->_ev.data.ptr = stat;
	epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);
}

