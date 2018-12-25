/*
 *	=> tcp_server  程序
 *	   发送信息给客户端
 *	   接收客户端发的消息
 *		
 *	
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "print_err.h"		//返回错误
#include <pthread.h>		//多线程
#include <signal.h>		//信号量 SIGINT

#define SPORT 8989		// 端口
#define SIP "39.108.103.169"	// ip

int cli_fd = -1;		//存放与连接客户端通信的描述符

void signal_fun(int signo)	//断开连接函数
{
	if(signo == SIGINT)
	{
		//断开连接
		//close(cli_fd);
		shutdown(cli_fd, SHUT_RDWR);

		//exit(-1);
		exit(0);
	}
}

/* 次线程接收客户端的信息 */
void *pth_fun(void *pth_arg)
{
	int ret = 0;
	//Data stu_data = {0};
	char r_data[100];

	while(1)
	{
		//ret = read(cli_fd, &stu_data, sizeof(stu_data));
		ret = recv(cli_fd, &r_data, sizeof(r_data), 0);	//接收客户端信息
		if(ret == -1)
		{
			print_err("recv fail", __LINE__, errno);
		}
		else if(ret > 0)
		{
			printf("\n[客户端] %s\n", r_data);
		}
	}
}

int main(void)
{
	int sockfd = -1;
	
	printf("=> 启动服务器程序 \n");

    	/* 创建使用 TCP 协议通信的套接字文件 */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) 
    	{
        	print_err("socket fail", __LINE__,errno);
    	}
    	else
    	{
        	printf(" -> 创建套接字\n");
    	}

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;                //制定 ip 地址格式（协议族）
	saddr.sin_port   = htons(SPORT);           //服务器端口
	saddr.sin_addr.s_addr = inet_addr(SIP);    //服务器 ip



    	/* 调用 bind 绑定套接字文件 ip 端口 */
	int ret = -1;    
	ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
	if(ret == -1) 
	{
		print_err("bind fail", __LINE__, errno);
	}
	else
	{
		printf(" -> 绑定套接字\n");
	}



	/* 将主动的 “套接字文件描述符” 转为被动，用于被动监听客户的连接 */
	ret = listen(sockfd, 3);
	if(ret == -1)
	{
		print_err("listen fail", __LINE__, errno);
	}
	else
	{
		printf(" -> 转换为被动监听\n");
		printf(" -> 等待连接......\n");
	}



	/* 调用 accept 函数，被动监听客户端的连接 */
	struct sockaddr_in cli_addr = {0};	//存放客户的 ip 和 端口
	int cli_addr_size = sizeof(cli_addr);
	cli_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_addr_size);
	if(cli_fd == -1)
	{
		print_err("accept fail", __LINE__, errno);
	}
	else
	{
		printf(" -> 监听客户端的连接\n");
	}

	/* 打印客户端的 ip 和 端口，一定要记得进行端序转换 */
	printf("=> 客户端[%s][%d] 连接\n", inet_ntoa(cli_addr.sin_addr),\
		ntohs(cli_addr.sin_port));


	/* 创建一个次线程，用于接收数据 */
	pthread_t tid;
	ret = pthread_create(&tid, NULL, pth_fun, NULL);
	if(ret != 0)
	{
		print_err("pthread_create fail", __LINE__, ret);
	}

	char data[100];
	int tmp_num;

	/* 向客户端发送数据 */
	while(1)
	{
		printf(">_:");
		scanf("%s", data);

		//ret = write(cli_fd, (void *)stu_data, sizeof(stu_data));	
		ret = send(cli_fd, (void *)&data, sizeof(data), 0);
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
