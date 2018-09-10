
#include <stdio.h>

void help(void){
printf(" \
单功能模块, shell 传入参数必须是: \n\
./sock_cli_once <server ip> <service port> <发送到srv 的数据>\n\n \
异步 = 0/同步 = 1\n \
不接受异常参数, 该版本不会自动解析dns 来获取port\n \
结果: 根据发送数据样板, 循环发/收/close 一次, 收到数据自动打印出来\n\n\n");
}

#ifdef _printf
#else
#define _printf(args...) printf(args)
#endif

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//socket 选项约定参数
//********************************************
//socket 协议簇类型
#define sock_proto AF_INET
//socket 类型(第三参数为0, 默认跟从创建...)
#define sock_type SOCK_STREAM

//接收缓冲区 = 64*1024 = 65536 = 64kb
#define sock_buf_recv 2048
//发送缓冲区
#define sock_buf_send 2048
//设置接收超时
#define sock_timeout_recv 5
//置发送超时
#define sock_timeout_send 5
//设置关闭时如果有数据未发送完, 等待1s 再关闭socket
#define sock_close_timeout 2

//发送10 次才io 才结束进程
#define test_times 10


//********************************************
//全局变量
unsigned int tmp;
#define ipaddr_len_max 64
char srv_ip[ipaddr_len_max];
unsigned srv_port;


//互交一次!!
#define send_data_size_max 8192
char sock_rw_buf[send_data_size_max];
char sock_rw_buf2[send_data_size_max];

bool test_once(char* send_data_buf);//执行自动化互交once 的函数


int main(int argc, char** argv){
  if(argc != 4){
    help();
    printf("\n目前参数个数为::<%d>\n\n", argc);
    return 0;
  }

  memset(&srv_ip, '\0', ipaddr_len_max);
  assert(argv[1] != NULL);
  strncpy(srv_ip,argv[1],ipaddr_len_max);

  srv_port = atoi(argv[2]);
  assert(srv_port != 0);

  memset(&sock_rw_buf2, '\0', send_data_size_max);
  assert(argv[3] != NULL);
  strncpy(sock_rw_buf2,argv[3],send_data_size_max);

  //输入值检测ok ..
  printf("tmp = %d, srv_ip = %s, srv_port = %d, send data = %s\n", tmp, srv_ip, srv_port, sock_rw_buf2);
  
	if(!test_once(sock_rw_buf2))
	  _printf("\ntest_once() fail!!\n");

  return 0;
}


//互交一次!!
bool test_once(char* send_data_buf){
	//创建socket
  int sfd = socket(sock_proto, sock_type, 0);
	if(sfd == -1){
		_printf("socket() fail, errno = %d\n", errno);
		return false;
	}

	//设置接收缓冲区
	int opt_val = sock_buf_recv;
	opt_val = setsockopt(sfd, SOL_SOCKET, SO_RCVBUF,\
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_revbuf() fail, errno = %d\n", errno);
		close(sfd);
		return false;
	}

	//设置发送缓冲区
	opt_val = sock_buf_send;
	opt_val = setsockopt(sfd, SOL_SOCKET, SO_SNDBUF, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_sndbuf() fail, errno = %d\n", errno);
		close(sfd);
		return false;
	}

	//设置接收超时
	struct timeval ptimeout;
	ptimeout.tv_usec = 0;
	ptimeout.tv_sec = sock_timeout_recv;
	opt_val = setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO,\
							&ptimeout, sizeof(struct timeval));
	if(opt_val == -1){
		_printf("set_sockopt_revtimeout() fail, errno = %d\n", errno);
		close(sfd);
		return false;
	}

	//设置发送超时
	ptimeout.tv_sec = sock_timeout_send;
	opt_val = setsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO,\
							&ptimeout, sizeof(struct timeval));
	if(opt_val == -1){
		_printf("set_sockopt_sndtimeout() fail, errno = %d\n", errno);
		close(sfd);
		return false;
	}

	//设置关闭时如果有数据未发送完, 等待n 秒再关闭socket
	struct linger plinger;
	plinger.l_linger = sock_close_timeout;
	plinger.l_onoff = true;
	opt_val = setsockopt(sfd, SOL_SOCKET, SO_LINGER, \
							&plinger, sizeof(struct linger));
	if(opt_val == -1){
		_printf("set_sockopt_linger() fail, errno = %d\n", errno);
		close(sfd);
		return false;
	}

	//设置地址重用
	opt_val = true;
	opt_val = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,\
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_reuseaddr() fail, errno = %d\n", errno);
		close(sfd);
		return false;
	}

  //执行connect()
	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_addr.s_addr = inet_addr(srv_ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(srv_port);
	opt_val = connect(sfd, (struct sockaddr*)&addr,\
							sizeof(struct sockaddr));
  if(opt_val == -1){
		_printf("connect() fail, errno = %d\n", errno);
		close(sfd);
		return false;
  }

  //执行发送操作--flags=0 阻塞
	int _test	= 0;
	ssize_t len = 0;
	for(;_test < test_times;_test++){
  	len = send(sfd, send_data_buf, sizeof(sock_rw_buf2), 0);
  	if(len == -1){
  		_printf("send() fail, errno = %d\n", errno);
  		shutdown(sfd,2);
  		close(sfd);
    	return false;//socket 错误
  	}
		if(len == 0){//对端已经close
			_printf("each other closed already \
							when sfd = %d sending data...\n", sfd);
  		shutdown(sfd,2);
  		close(sfd);
			return true;
		}
  	_printf("send data: %s\n", send_data_buf);

  	//接收数据(肯定要同步, 否则没有数据可以显示, 但是有recv_timeout 限制)
		memset(&sock_rw_buf, '\0', sizeof(sock_rw_buf));
		len = recv(sfd, sock_rw_buf, sizeof(sock_rw_buf), 0);
		if(len == -1){
			if(errno != EAGAIN){
				shutdown(sfd,2);
				close(sfd);
				return false;//读取socket 错误
			}
		}
		if(len == 0){//对端已经close
			_printf("each other closed already \
							when sfd = %d recving data...\n", sfd);
			shutdown(sfd,2);
			close(sfd);
			return true;
		}

		//互交正确, 打印数据, 关闭socket
		_printf("recv data: %s\n", sock_rw_buf);
		sleep(1);
	}
  //********************************************
  //多发一次
	//执行发送操作--flags=0 阻塞
  len = send(sfd, send_data_buf, sizeof(sock_rw_buf2), 0);
  if(len == -1){
  	_printf("send() fail, errno = %d\n", errno);
  	shutdown(sfd,2);
  	close(sfd);
    return false;//socket 错误
  }
	if(len == 0){//对端已经close
		_printf("each other closed already \
						when sfd = %d sending data...\n", sfd);
  	shutdown(sfd,2);
  	close(sfd);
		return true;
	}
  _printf("send data: %s\n", send_data_buf);

  //接收数据(肯定要同步, 否则没有数据可以显示, 但是有recv_timeout 限制)
	memset(&sock_rw_buf, '\0', sizeof(sock_rw_buf));
	len = recv(sfd, sock_rw_buf, sizeof(sock_rw_buf), 0);
	if(len == -1){
		if(errno != EAGAIN){
			shutdown(sfd,2);
			close(sfd);
			return false;//读取socket 错误
		}
	}
	if(len == 0){//对端已经close
		_printf("each other closed already \
						when sfd = %d recving data...\n", sfd);
		shutdown(sfd,2);
		close(sfd);
		return true;
	}

	//互交正确, 打印数据, 关闭socket
	_printf("recv data: %s\n", sock_rw_buf);
	//**********************************************
	//结束清空资源
	shutdown(sfd,2);
	close(sfd);
	return true;
}
