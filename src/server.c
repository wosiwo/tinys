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

//tyWorker变量保存worker进程信息
tyWorker  workers[WORKER_NUM];
tyWorker worker;
//tyReactor 保存reactor线程信息
tyReactor  reactors[REACTOR_NUM];

//通过连接fd，获取主进程管道(后续用于获取reactor线程管道)
int connFd2WorkerId[1000];
//ev用于注册事件,数组用于回传要处理的事件
struct epoll_event ev,events[20];
int i, maxi, listenfd, new_fd, sockfd,epfd,nfds;

//将某个描述符注册为可写
void setOutPut(char * data,int fd,int length){
	printf("setOutPut fd %d \n",fd);
	printf("epfd fd %d \n",epfd);
	resLength =length;
	memcpy(response, data, resLength);
	//strcpy(response, data);	//data 中包含 \0(可能) 不能使用strcpy
	//设置用于写操作的文件描述符
	ev.data.fd=fd;
	//设置用于注测的写操作事件
	ev.events=EPOLLOUT|EPOLLET;
	//修改sockfd上要处理的事件为EPOLLOUT
	epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);
}

//创建epoll
int epollCreate(){
	int tmpEpFd;
    //生成用于处理accept的epoll专用的文件描述符
	tmpEpFd=epoll_create(512);
    return tmpEpFd;
}

//添加监听事件
int epollAdd(int epollfd,int fd, int eventType){
	struct epoll_event e;
	setnonblocking(fd);
	//设置与要处理的事件相关的文件描述符
	e.data.fd=fd;
	//设置要处理的事件类型
	e.events=eventType;
	//注册epoll事件
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&e);
	return SW_OK;
}

//修改监听事件
int epollEventSet(int epollfd, int fd, int eventType) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = eventType;
    int r = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    return SW_OK;
}

/**
 * ReactorThread main Loop
 * 线程循环内容
 */
int swReactorThread_loop(int reactor_id)
{
	int pipe_fd;
	int epollfd;
	int sockfd;
	struct epoll_event local_events[20];
	//线程id
	pthread_t thread_id = pthread_self();
	//创建epoll
	epollfd = epollCreate();

	printf("swReactorThread_loop reactor_id %d epollfd %d \n",reactor_id,epollfd);

	//监听当前线程对应的worker进程的pipMasterFd
	//TODO 待实现 一个reactor线程监听多个worker线程的管道，该如何区分客户端连接的fd,与pipe的fd
	pipe_fd = reactorMapWorker(reactor_id,epollfd);

	//存储线程相关信息
	reactors[reactor_id].pidt = thread_id;
	reactors[reactor_id].epfd = epollfd;

	//后续主进程接受到连接时，会为某个reactor线程添加连接的监听事件
	int nfds,i, pipefd,ret;
	ssize_t n;
	char line[MAXLENGTH];
	//循环等待事件触发
	while(1)
	{
		/* epoll_wait：等待epoll事件的发生，并将发生的sokct fd和事件类型放入到events数组中；
		* nfds：为发生的事件的个数。可用描述符数量
		* 注：
		*/
		nfds=epoll_wait(epollfd,local_events,20,500);
		//处理可用描述符的事件
		for(i=0;i<nfds;++i)
		{
			//TODO 区分主进程抛过来的连接，还是worker进程写回的数据
			//不是worker返回数据，则认为是主进程添加的连接监听
			if(local_events[i].events&EPOLLIN && local_events[i].data.fd!=pipe_fd)
			{
				sockfd = local_events[i].data.fd;
				printf("connfd sockfd %d",sockfd);
				if ( (n = recv(sockfd, line, MAXLENGTH, 0)) < 0){
					if (errno == ECONNRESET)
					{
						close(sockfd);
						local_events[i].data.fd = -1;
					}else{
						printf("readline error");
					}
				}else if (n == 0){
						printf("read error \n");
						close(sockfd);
						local_events[i].data.fd = -1;
				}
				printf("line1 %c \n",line[20]);
				printf("line2 %zu n %zu \n",sizeof(line),n);

				//发送数据给worker进程
				reactorSend2Worker( epollfd, sockfd, line,  n);
			}else if(local_events[i].events&EPOLLIN){	//接受worker进程返回的数据
				printf("master rec worker pipe \n");
				//TODO worker进程返回的数据，接收完发给客户端
				swEventData task;
				int n;

				if ((n=recv(local_events[i].data.fd, &task, sizeof(task), 0)) > 0)
				{
					//TODO 判断是否可以直接输出，还是必须修改epoll事件状态
					ret =  write(task.info.from_fd, task.data, task.info.len);
					printf("ret %d \n",ret);
					if (ret<0)
					{
						printf("errno %d \n",errno);
					}
					close( task.info.from_fd );
				}
			}
		}
	}
}

//server启动
void runServer(char* ip,int port){
	int ret;
	int pid =getpid();
	int forkPid ;
	int mainEpollFd;
	printf("pid  %d\n",pid);

	//创建manager进程及其下的各个worker子进程
	int workerNum ;
	workerNum = WORKER_NUM;
	forkPid = manageProccess(workerNum);
	if(forkPid==0){
		return 0;
	}
	// 获取pid
	int pid1 =getpid();
	printf("pid1  %d\n",pid1);

	mainEpollFd = mainReactorRun(ip, port);

	// 主进程创建各个reactor线程，之后主进程循环监听listen事件accept后抛给reactor线程处理，
	//创建reactor线程
	int reactorNum = 2;
	pthread_t pidt;
	for (i = 0; i < reactorNum; i++)
	{
		if (pthread_create(&pidt, NULL,swReactorThread_loop, i) < 0)
		{
			printf("pthread_create[tcp_reactor] failed. Error: %s[%d]", strerror(errno), errno);
		}
		printf("pthread_create  %d pidt %d \n",i,pidt);
	}
	//主进程开始循环监听
	mainReactorWait(mainEpollFd);
}


//服务主进程启动接受链接
int mainReactorRun(char* ip,int port)
{
	printf("mainReactorRun ip %s port %d \n",ip,port);

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
    epfd = epollCreate();
    //添加监听事件，监听端口
    int fdtype =EPOLLIN|EPOLLET;
    epollAdd(epfd,listenfd,fdtype);

    //设置服务器端地址信息
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    //监听的地址
    char *local_addr= ip;
    inet_aton(local_addr,&(serveraddr.sin_addr));
    serveraddr.sin_port=htons(port);
    //绑定socket连接
    bind(listenfd, ( struct sockaddr* )&serveraddr, sizeof(serveraddr));
    //监听
    listen(listenfd, LISTENQ);
    maxi = 0;
    printf("listenfd %d epfd %d \n",listenfd,epfd);

    return listenfd;
}

int mainReactorWait(int mainEpollFd){
	int readfd,writefd;
	int nfds;
	socklen_t clilen;
	struct sockaddr_in clientaddr;
	while(1)
	{
		/* epoll_wait：等待epoll事件的发生，并将发生的sokct fd和事件类型放入到events数组中；
		* nfds：为发生的事件的个数。可用描述符数量
		*/
		nfds=epoll_wait(epfd,events,20,500);
		//处理可用描述符的事件
		for(i=0;i<nfds;++i)
		{
				//当监听端口描述符可用时，接收链接的时候
			if(events[i].data.fd==mainEpollFd)
			{
					/* 获取发生事件端口信息，存于clientaddr中；
				*new_fd：返回的新的socket描述符，用它来对该事件进行recv/send操作*/
				new_fd = accept(mainEpollFd,(struct sockaddr *)&clientaddr, &clilen);
				printf("new_fd %d \n",new_fd);
				if(new_fd<0)
			   {
					perror("new_fd<0\n");
					return 1;
				}
				perror("setnonblocking\n");
				setnonblocking(new_fd);
				char *str = inet_ntoa(clientaddr.sin_addr);

				//给reactor线程添加监听事件，监听本次连接
				int reactor_id = new_fd%REACTOR_NUM; //连接fd对REACTOR_NUM取余，决定抛给哪个reactor线程
				int reactor_epfd = reactors[reactor_id].epfd;
				printf("reactor_epfd %d new_fd %d \n",reactor_epfd,new_fd);
				int fdtype =EPOLLIN|EPOLLET;
				epollAdd(reactor_epfd,new_fd,fdtype);
			}
		}
	}
}

/**
 * 将php返回的数据，写入pipWorkerFd管道中，由监听该管的reactor线程发送给客户端
 */
int send2ReactorPipe(char * data,int fd,int length){
	int ret;
	swEventData task;
	task.info.from_fd = fd;
	task.info.len = length;
	memcpy(task.data, data, length);

	//对应的reactor线程在监听pipMasterFd所以写入pipWorkerFd
	int workerPipe = worker.pipWorkerFd;

	printf("send2ReactorPipe fd %d \n",fd);
	printf("send2ReactorPipe workerPipe %d \n",workerPipe);
	ret = write(workerPipe, &task, sizeof(task));

	return 1;
}
