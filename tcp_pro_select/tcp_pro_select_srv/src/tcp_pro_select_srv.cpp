//同步-select-单次io <--> tcp 回显服务器

#include <sys/time.h> // for select
#include <sys/types.h>
#include <unistd.h>

#include <sys/types.h> // for socket
#include <sys/socket.h>
#include <sys/socket.h> // for shutdown

#include <netinet/in.h> // for inet_addr
#include <arpa/inet.h>

#include <stdio.h>
#include <strings.h> // for bzero
#include <string.h> // for strlen
#include <assert.h>
#include <errno.h>
#include <unistd.h> // for fork
#include <fcntl.h> // for fcntl
#include <stdlib.h> // for exit

//#include <sys/types.h>
#include <sys/wait.h> // for waitpid

//宏声明--#define _printf(args...) assert(printf(args) != -1);
//makefile 把断言禁用掉了, 但是打印输入本来就不该嵌入断言, 
//否则当你禁用断言的时候, 这个打印输出就消失了, 断言滥用了.
#ifdef _printf
#else
	#define _printf(args...) printf(args)
#endif

#define sfd_max 1024 //socket fd max = 1024, 一个进程一个socket
#define pro_max 1024

#define io_buf_max 2048
#define io_sock_close_timeout 2

#define srv_ip "127.0.0.1"
#define srv_port 6666
#define BACKLOG 1024 //监听队列最大长度 1024

#define __max(a,b) (a>b ? a : b)

//全局变量声明列表:
typedef struct g_srv{
  bool is_working;       //socket 服务是否正常工作
  int pid_pool[pro_max];
	int pro_count;				 //子进程的数量count
  int err_count;
	int test_count;

	unsigned int sfd_li;   //监听sfd
  int sfd_cur_max;       //select 中fd最大的一个值
  struct timeval timeout;//timeout 变量
  fd_set r_set;
  fd_set err_set;

}g_srv_t;
g_srv_t _g_srv;//唯一全局变量


//前置函数声明列表
bool fork_mission(void);//fork 任务函数
bool do_mission(int sfd_acc);//执行一次任务




//主函数
int main(void){
  _printf("\n program start\n\n");
  //初始化全局变量
  bzero(&_g_srv, sizeof(g_srv_t));

  //创建监听socket
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1){
		_printf("socket() fail, errno = %d\n", errno);
		return -1;
	}
  //设置监听socket 选项
  //设置地址重用
  int opt_val = true;
  opt_val = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_reuseaddr() fail, errno = %d\n", errno);
		close(sfd);
		return -1;
	}

  //执行bind
	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_addr.s_addr = inet_addr(srv_ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(srv_port);

  opt_val = bind(sfd, (struct sockaddr*)&addr, \
							sizeof(struct sockaddr));
  if(opt_val == -1){
    _printf("socket %d bind fail, errno: %d\n", sfd, errno);
    close(sfd);
    return -1;
  }

  //执行listen
  opt_val = listen(sfd, BACKLOG);
  if(opt_val == -1){
    _printf("socket %d listen fail, errno: %d\n", sfd, errno);
    close(sfd);
    return -1;
  }
  _g_srv.sfd_li = sfd;
	_g_srv.is_working = true;

  //工作循环
  while(_g_srv.is_working){
    //查看监听sfd_li 有没有读事件
		FD_ZERO(&_g_srv.r_set);
    FD_SET(_g_srv.sfd_li, &_g_srv.r_set);
    _g_srv.sfd_cur_max = __max(_g_srv.sfd_li,_g_srv.sfd_cur_max);    
		opt_val = select(_g_srv.sfd_cur_max + 1, &_g_srv.r_set, \
								NULL, NULL, &_g_srv.timeout);
		if(opt_val == -1){//select 错误, 终止程序
      _printf("select() fail, errno: %d\n", errno);
			_g_srv.is_working = false;
			continue;
		}
		if(opt_val == 0){//暂时没有数据操作, pass
			;
		}
		if(opt_val > 0){//select 有opt_val 个sfd 响应<读事件>
      _printf("select return %d sfd has read event\n", opt_val);
			opt_val = FD_ISSET(_g_srv.sfd_li, &_g_srv.r_set);
      if(opt_val == 0){//未知错误
				_printf("triggering select is not listen sfd %d, \
					unknow error !! program going down\n", _g_srv.sfd_li);
				_g_srv.is_working = false;
				continue;
			}
			else{//有新客户端链接到服务器
				if(!fork_mission()){
					_printf("fork_mission() fail, \
						err_count = %d\n", _g_srv.err_count);
					continue;
				}
			}
		}


    //查看监听sfd_li 有没有网络错误
		FD_ZERO(&_g_srv.err_set);
    FD_SET(_g_srv.sfd_li, &_g_srv.err_set);
    _g_srv.sfd_cur_max = __max(_g_srv.sfd_li,_g_srv.sfd_cur_max);
    opt_val = select(_g_srv.sfd_cur_max + 1, NULL, \
								NULL, &_g_srv.err_set, &_g_srv.timeout);
		if(opt_val == -1){//select 错误, 终止程序
      _printf("select() fail, errno: %d\n", errno);
			_g_srv.is_working = false;
			continue;
		}
		if(opt_val == 0){//暂时没有数据操作, pass
			;
		}
		if(opt_val > 0){//select 有opt_val 个sfd 响应<错误事件>
      _printf("select return %d sfd has error event\n", opt_val);
			opt_val = FD_ISSET(_g_srv.sfd_li, &_g_srv.err_set);
      if(opt_val == 0){//未知错误
				_printf("triggering select is not listen sfd %d, \
					unknow error !! program going down\n", _g_srv.sfd_li);
				_g_srv.is_working = false;
				continue;
			}
			else{
				_printf("listen sfd %d just occurred An error, \
					program going to quit\n", _g_srv.sfd_li);
				_g_srv.is_working = false;
				continue;
			}
		}

		
		//检查线程池哪个子进程已经结束
    if(_g_srv.pro_count > 0){
		  int tmp = 0;
			int son_ret_stat;
			//查找已经使用的pid 位置
		  for(;_g_srv.pid_pool[tmp] != 0 && _g_srv.pro_count != 0;tmp++){
				opt_val = waitpid(_g_srv.pid_pool[tmp], \
										&son_ret_stat, WNOHANG);
				if(opt_val == -1){//waitpid 失败
					_printf("waitpid() fail, errno = %d!!\n", errno);
					_g_srv.is_working = false;
					continue;
				}
				if(_g_srv.is_working == true)//主进程要退出?
					break;
				if(opt_val == 0)//子进程还没结束
					continue;
				_printf("son process %d had quit, return value = %d\n",\
					opt_val, son_ret_stat);
				_g_srv.pid_pool[tmp] = 0;//还原线程池
				_g_srv.pro_count--;//计数-1
				_printf("son process count now is: %d\n", _g_srv.pro_count);
			}
		}

  }//while(_g_srv.is_working) end


  //清空资源
  shutdown(_g_srv.sfd_li,2);
	close(_g_srv.sfd_li);
  _printf("\nprogram going to quit\nbye!!\n\n");
  return 0;
}

//fork 任务函数
bool fork_mission(void){
  struct sockaddr addr;
	socklen_t addr_len = sizeof(struct sockaddr);
	bzero(&addr, sizeof(struct sockaddr_in));
	//接受客户端
	int sfd_acc = accept(_g_srv.sfd_li, &addr, &addr_len);
	if(sfd_acc == -1){
		_printf("accept() fail, errno: %d\n", errno);
		return false;
	}
	else{//fork 子进程, 去执行任务
		int tmp = 0;    
		for(;_g_srv.pid_pool[tmp] != 0;tmp++)//查找未被使用的pid 位置
			;
		if(tmp == pro_max - 1){//进程池已经满了
			_printf("process pool has full, fork_mission fail\n");
			shutdown(sfd_acc,2);
			close(sfd_acc);
			return false;
		}
		//设置close-on-exec标志
		fcntl(sfd_acc, FD_CLOEXEC, true);

		_g_srv.pid_pool[tmp] = fork();
    if(_g_srv.pid_pool[tmp] == -1){
			_printf("fork fail, errno = %d\n", errno);
			shutdown(sfd_acc,2);
			close(sfd_acc);
			return false;
		}
		if(_g_srv.pid_pool[tmp] == 0){
    	//当前新建的子进程
			if(!do_mission(sfd_acc))
				_printf("son process %d do_mission() fail !!\n", getpid());
			shutdown(sfd_acc,2);
			close(sfd_acc);
			exit(0);//子进程正常退出
    }
    else{
    	//父进程需要做的事
    	_g_srv.pid_pool[tmp] = tmp;
			_g_srv.pro_count++;
			_g_srv.test_count++;
			return true;
    }
	}

  _printf("unknow error in fork_mission()\n");
  return false;//未知错误
}


//执行一次任务
bool do_mission(int sfd_acc){
	//设置接收缓冲区
	int opt_val = io_buf_max;
	opt_val = setsockopt(sfd_acc, SOL_SOCKET, SO_RCVBUF, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_revbuf() fail, errno = %d\n", errno);
		return false;
	}
	//设置发送缓冲区
	opt_val = io_buf_max;
  opt_val = setsockopt(sfd_acc, SOL_SOCKET, SO_SNDBUF, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_sndbuf() fail, errno = %d\n", errno);
		return false;
	}

	//设置关闭时如果有数据未发送完, 等待n 秒再关闭socket
	struct linger plinger;
	plinger.l_linger = io_sock_close_timeout;
	plinger.l_onoff = true;
	opt_val = setsockopt(sfd_acc, SOL_SOCKET, SO_LINGER, \
							&plinger, sizeof(struct linger));
	if(opt_val == -1){
		_printf("set_sockopt_linger() fail, errno = %d\n", errno);
		return false;
	}

	//设置地址重用
	opt_val = true;
	opt_val = setsockopt(sfd_acc, SOL_SOCKET, SO_REUSEADDR, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_reuseaddr() fail, errno = %d\n", errno);
		return false;
	}

	char xbuf[io_buf_max];
	bzero(&xbuf, io_buf_max);
	//接收一次客户端数据
	opt_val = recv(sfd_acc, &xbuf, io_buf_max, 0);
	if(opt_val == 0){//对端已经关闭
		_printf("client has close when srv recving data\n");
		return true;
	}
	if(opt_val == -1){//recv 错误
		_printf("recv() fail, errno = %d\n", errno);
		return false;
	}
	if(opt_val > 0){//收到数据
		;
	}

  //显示一次客户端数据
	_printf("recv data from client:%s\n", xbuf);

	//组装回送数据
	bzero(&xbuf, io_buf_max);
	sprintf(xbuf, \
		"hello client, you are the %d\n", _g_srv.test_count);

  //回送一次客户端数据
	opt_val = send(sfd_acc, &xbuf, strlen(xbuf), 0);
	if(opt_val == 0){//对端已经关闭
		_printf("client has close when srv sending data\n");
		return true;
	}
	if(opt_val == -1){//recv 错误
		_printf("send() fail, errno = %d\n", errno);
		return false;
	}
	if(opt_val > 0){//回送数据成功
		;
	}

	_printf("return %d finish\n", _g_srv.test_count);
	return true;
}
