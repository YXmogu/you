/*
 *	=> tcp_client 程序
 *	   发送信息给服务器
 *	   接收服务器信息
 *
 *	-> youxia
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "print_err.h"

#define SIP "172.25.143.46"
#define SPORT 8989

int sock_fd = -1;

void signal_fun(int signo)
{
	if(SIGINT == signo)
	{
		/* 断开连接 */
		close(sock_fd);
		//shutdown(sock_fd);
		exit(0);
	}

}

void *pth_fun(void *pth_arg)
{
	int ret = 0;

	signal(SIGINT, signal_fun);
	
	char r_data[100];

	while(1)
	{
		/* 接收数据 */
		ret = recv(sock_fd, (void *)&r_data, sizeof(r_data), 0);
		if(ret > 0)
		{
			printf("\n[服务器] %s\n", r_data);
		}
		else if(ret == -1)
		{
			print_err("recv fail", __LINE__, errno);
		}
	}
}


int main()
{

	/* 
	 * 创建套接字文件，并指定使用 TCP 协议
	 * 对于客户端的套接字文件描述符，直接用于通信
	 */
	sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1)
	{
		print_err("socket fail", __LINE__, errno);
	}

	struct sockaddr_in ser_addr = {0};	//用于存放要连接的服务器 ip 和 端口

	ser_addr.sin_family = AF_INET;		//地址族
	ser_addr.sin_port = htons(SPORT);	//服务器程序的端口
	ser_addr.sin_addr.s_addr = inet_addr(SIP);	//服务器程序的 ip

	/* 调用 connect , 向服务器主动请求连接 */
	int ret = 0;
	ret = connect(sock_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if(ret == -1)
	{
		print_err("connect fail", __LINE__, errno);
	}

	/* 创建一个线程接收数据 */
	pthread_t tid;
	ret = pthread_create(&tid, NULL, pth_fun, NULL);
	if(ret != 0)
	{
		print_err("pthread fail", __LINE__, ret);
	}

	int tmp_num = 0;
	char data[100];		//数据 数组
	
	while(1)
	{
		printf(">_:");
		scanf("%s", &data);
		/* 发送数据 */
		ret = send(sock_fd, (void *)&data, sizeof(data), 0);
		if(ret == -1)
		{
			print_err("send fail", __LINE__, errno);
		}
		else
		{
			printf(" -> 发送成功\n");
		}
	}

	return 0;
}
