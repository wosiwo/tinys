
//TODO
/**
 * 	1. new socked
 * 	2. new listen
 * 	3. bind port
 * 	4. epoll wait
 * 	5. epoll_wait>0 then read
 * 	6. throw to php
 * 	7.
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
//#include <cassert>
#include <sys/epoll.h>
//#include <sys/event.h>
#include "ty_server.h"

//设置非阻塞描述符
int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}


char response[MAXLENGTH];
int resLength;

#define WORKER_NUM 20

//tyWorker变量保存worker进程信息
tyWorker  workers[WORKER_NUM];

struct epoll_event ev,events[20];//ev用于注册事件,数组用于回传要处理的事件
int i, maxi, listenfd, new_fd, sockfd,epfd,nfds;

int setOutPut(char * data,int fd,int length){
	printf("setOutPut fd %d \n",fd);
	printf("epfd fd %d \n",epfd);

	resLength =length;

	memcpy(response, data, resLength);
	//strcpy(response, data);	//data 中包含 \0(可能) 不能使用strcpy
	ev.data.fd=fd;//设置用于写操作的文件描述符
	ev.events=EPOLLOUT|EPOLLET;//设置用于注测的写操作事件
	epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);//修改sockfd上要处理的事件为EPOLLOUT
}

int epollCreate(){

    //生成用于处理accept的epoll专用的文件描述符
    epfd=epoll_create(256);
    return epfd;
}

int epollAdd(epollfd,readfd){
	struct epoll_event e;
	setnonblocking(readfd,epollfd);
	//生成用于处理accept的epoll专用的文件描述符
	epfd=epoll_create(256);
	//设置与要处理的事件相关的文件描述符
	e.data.fd=listenfd;
	//设置要处理的事件类型
	e.events=SW_FD_PIPE | SW_EVENT_READ;
	//注册epoll事件
	epoll_ctl(epollfd,EPOLL_CTL_ADD,readfd,&e);

	return SW_OK;
}


int runServer(char* ip,int port){
	//创建manager进程及其下的各个worker子进程
	int workerNum ;
	workerNum = WORKER_NUM;
	manageProccess(workerNum);


	//TODO 主进程创建各个reactor线程，之后主进程循环监听listen事件accept后抛给reactor线程处理，

	//TODO 第一步实现：主进程中直接取出数据，抛给worker进程
	mainReactorRun(ip, port);



//	return server(ip, port);
}


//服务启动
int mainReactorRun(char* ip,int port)
{


	//int i, maxi, listenfd, new_fd, sockfd,epfd,nfds;
    ssize_t n;
    char line[MAXLENGTH];
    socklen_t clilen;
    //struct epoll_event ev,events[20];//ev用于注册事件,数组用于回传要处理的事件
    struct sockaddr_in clientaddr, serveraddr;
    //生成socket文件描述符
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //把socket设置为非阻塞方式
    setnonblocking(listenfd);
    //生成用于处理accept的epoll专用的文件描述符
    epfd=epoll_create(256);
    //设置与要处理的事件相关的文件描述符
    ev.data.fd=listenfd;
    //设置要处理的事件类型
    ev.events=EPOLLIN|EPOLLET;
    //注册epoll事件
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
    printf("step 1 \n");
    //设置服务器端地址信息
    bzero(&serveraddr, sizeof(serveraddr));
    printf("step 2 \n");
    serveraddr.sin_family = AF_INET;
    //监听的地址
    char *local_addr= ip;
    printf("step 3 \n");
    inet_aton(local_addr,&(serveraddr.sin_addr));
    printf("step 3.1 \n");
    serveraddr.sin_port=htons(port);
    printf("step 3.2 \n");
    //绑定socket连接
    bind(listenfd, ( struct sockaddr* )&serveraddr, sizeof(serveraddr));
    printf("step 4 \n");
    //监听
    listen(listenfd, LISTENQ);
    maxi = 0;
    printf("listenfd %d \n",listenfd);
    while(1)
    {
		/* epoll_wait：等待epoll事件的发生，并将发生的sokct fd和事件类型放入到events数组中；
		* nfds：为发生的事件的个数。可用描述符数量
		* 注：
		*/
        nfds=epoll_wait(epfd,events,20,500);
        //处理可用描述符的事件
        for(i=0;i<nfds;++i)
        {
        		printf("nfds %d \n",nfds);
        		//当监听端口描述符可用时，接收链接的时候
            if(events[i].data.fd==listenfd)
            {
            		/* 获取发生事件端口信息，存于clientaddr中；
                *new_fd：返回的新的socket描述符，用它来对该事件进行recv/send操作*/
                new_fd = accept(listenfd,(struct sockaddr *)&clientaddr, &clilen);
                printf("new_fd %d \n",new_fd);
                if(new_fd<0)
			   {
                    perror("new_fd<0\n");
                    perror("test\n");
                    return 1;
                }
                perror("setnonblocking\n");
                setnonblocking(new_fd);
                char *str = inet_ntoa(clientaddr.sin_addr);
                //设置用于读操作的文件描述符
                ev.data.fd=new_fd;
                //设置用于注测的读操作事件
                ev.events=EPOLLIN|EPOLLET;
                epoll_ctl( epfd, EPOLL_CTL_ADD, new_fd, &ev );
            }
			else if(events[i].events&EPOLLIN)//当数据进入触发下面的流程
			{
				sockfd = events[i].data.fd;
				printf("read 0\n");
				if ( (n = recv(sockfd, line, MAXLENGTH, 0)) < 0){
						printf("read n  \n");
					if (errno == ECONNRESET)
					{
						close(sockfd);
						events[i].data.fd = -1;
					}else{
						printf("readline error");
					}
				}else if (n == 0){
						printf("read error \n");
						close(sockfd);
						events[i].data.fd = -1;
				}
				printf("line %s \n",line);
				printf("line1 %c \n",line[20]);
				printf("line2 %zu \n",sizeof(line));
				printf("n %zu \n",n);
				//设置用于写操作的文件描述符
				ev.data.fd=sockfd;
				ev.events=EPOLLOUT|EPOLLET;//设置用于注测的写操作事件
	//              epoll_ctl(epfd,ev.events,sockfd,&ev);//修改sockfd上要处理的事件为EPOLLOUT

				//将接受到的请求抛给PHP
	//                	php_tinys_onReceive(sockfd,line,n);
					//TODO 写入管道,抛给worker子进程
				//取取余数获取worker进程，将数据写入其监听的管道中
				int i = sockfd%WORKER_NUM;
				int pipeWriteFd = workers[i].pipWriteFd;
				int ret;

				swEventData task;
				task.info.from_fd = sockfd;
				//TODO 后续多个reactor线程，需要记录线程id
				// 需要写一个结构体，传入连接fd
				memcpy(task.data, line, n);
				ret = write(pipeWriteFd, task, sizeof(task));

			}
			else if(events[i].events&EPOLLOUT)//当数据发送触发下面的流程
			{
				sockfd = events[i].data.fd;
				printf("response length %d \n",resLength);
				int  ret;
				printf("wirte data fd %d \n",sockfd);
				printf("response %s \n",response);
				printf("res char %c \n",response[20]);
				printf("res char %c \n",response[1]);
				printf("res char %c \n",response[10]);
				ret =  write(sockfd, response, resLength);
				printf("ret %d \n",ret);
				if (ret<0)
			{
				printf("errno %d \n",errno);
			}
				ev.data.fd=sockfd;//设置用于读操作的文件描述符
				ev.events=EPOLLET;//设置用于注测的读操作事件 EPOLLIN|
				//EPOLL_CTL_DEL
				epoll_ctl( epfd, EPOLL_CTL_DEL, sockfd, 0 );
				close( sockfd );
				//epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);//修改sockfd上要处理的事件为EPOLIN
			}
        }
    }
}
