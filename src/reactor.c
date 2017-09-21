#include "ty_server.h"

//引用worker各个子进程的的信息变量
extern tyWorker workers[WORKER_NUM];

//tyReactor 保存reactor线程信息
tyReactor  reactors[REACTOR_NUM];

/**
 * 监听当前线程对应的worker进程的pipMasterFd
 */
int reactorMapWorker(int reactor_id,int epollfd){
	int pipe_fd;
	int i;
	//监听某个worker进程的pipemasterfd
	for(i = 0; i < WORKER_NUM; i++){
		if (i % REACTOR_NUM == reactor_id)
		{
			pipe_fd = workers[i].pipMasterFd;
			//添加监听事件，监听管道
			int fdtype = EPOLLIN|EPOLLET;
			//TODO fdtype 是否需要转化 swReactorEpoll_event_set
			printf("reactor_id %d worker_id %d pipe_fd %d \n",reactor_id,i,pipe_fd);
			epollAdd(epollfd,pipe_fd,fdtype);
		}
	}
	return pipe_fd;
}

//发送数据给worker进程
int reactorSend2Worker(int epollfd,int sockfd,char * line, ssize_t n){
	//TODO 写入管道,抛给worker子进程
	//取取余数获取worker进程，将数据写入其监听的管道中
	int i = sockfd%WORKER_NUM;
	printf(" rand worker id %d \n",i);
	int pipeWriteFd = workers[i].pipMasterFd;
	int ret;

	swEventData task;
	task.info.from_fd = sockfd;
	task.info.len = n;

	//TODO 后续多个reactor线程，需要记录线程id
	// 需要写一个结构体，传入连接fd
	memcpy(task.data, line, n);

	printf("write worker_pipe_fd fd %d \n",pipeWriteFd);

	//抛给worker进程后就不再监听本次连接
	epoll_ctl( epollfd, EPOLL_CTL_DEL, sockfd, 0 );

	ret = write(pipeWriteFd, &task, sizeof(task));
	return 1;
}
