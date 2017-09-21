#include <sys/socket.h>
#include "ty_server.h"

//引用worker各个子进程的的信息变量
extern tyWorker workers[WORKER_NUM];

//各个worker子进程中保存本进程信息
extern tyWorker worker;

/**
 * 创建各个worker子进程的管道
 */
int createWorkerPipe(int workerNum){
	int i;
	int ret;
	int socks[2];

	//需要再master进程中初始化好worker进程所用的管道，在manager进程初始化master进程取不到数据
	for (i = 0; i < workerNum; i++)
	{
		//在master进程中将所有管道都创建好
		ret = socketpair(AF_UNIX, SOCK_DGRAM, 0, socks);

		//获取用于读取的fd
		workers[i].pipWorkerFd = socks[1];
		workers[i].pipMasterFd = socks[0];
//		workers[i].pipeMasterWriteFd = masterSocks[0];
		printf("worker_id %d pipMasterFd %d \n",i,workers[i].pipMasterFd);
	}
	return 1;

}
/**
 * 创建manager进程
 */
int manageProccess(int workerNum)
{
	pid_t pid;
	pid_t subpid;
	int i;
	createWorkerPipe(workerNum);
	pid = fork();
	switch (pid)
	{
		//创建manager进程
		case 0:
			for (i = 0; i < workerNum; i++)
			{
				//close(worker_pipes[i].pipes[0]);
				subpid = tyManager_spawn_worker( i);
				if (subpid < 0)
				{
					printf("fork() failed.");
					return SW_ERR;
				}
				else
				{
					//TODO 全局变量保存各个子进程的pid
					workers[i].pid = pid;
				}
			}
			//manager循环
			//TODO manager进程管理功能
			while(1){
				sleep(60);
			}
			exit(0);
			break;
			//master process
		default:
			break;
	}
	return 1;
}

//work 执行内容
int tyWorker_loop(int worker_id){
	int ret;
	int pipefd;
	int readfd;
	struct epoll_event local_events[20];
	int epollfd;
	//TODO 监听pipe管道事件
	//取出在manager进程中创建好的管道
	readfd = workers[worker_id].pipWorkerFd;
    //创建epoll
    epollfd = epollCreate();
    //添加监听事件，监听管道
    int fdtype = EPOLLIN|EPOLLET;
    //TODO fdtype 是否需要转化 swReactorEpoll_event_set
    epollAdd(epollfd,readfd,fdtype);
    int nfds;
    int i;
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
			printf("tyWorker_loop nfds %d \n",nfds);
			//当监听端口描述符可用时，接收链接的时候
			if(local_events[i].events&EPOLLIN)//当数据进入触发下面的流程
			{
				pipefd = local_events[i].data.fd;
				//从pipe中读取数据
				swWorker_onPipeReceive(pipefd,worker_id);
			}
		}
	}
}

/**
 * 从管道中读取数据，并发送给php
 */
void swWorker_onPipeReceive(int fd,int worker_id){
    swEventData task;
//    char line[MAXLENGTH];
    int n;

	if ((n=recv(fd, &task, sizeof(task), 0)) > 0)
	{
		//需要将连接fd传给php
		php_tinys_onReceive(task.info.from_fd,task.data,task.info.len);
	}
}

//fork Worker进程
 pid_t tyManager_spawn_worker(int worker_id)
{
    pid_t pid;
    int ret;
    pid = fork();
    //fork() failed
    if (pid < 0)
    {
        swWarn("Fork Worker failed. Error: %s [%d]", strerror(errno), errno);
        return SW_ERR;
    }
    //worker child processor
    else if (pid == 0)
    {
		printf("worker child processor worker_id %d \n",worker_id);
		//TODO 监听pipe管道事件
		workers[worker_id].pid = pid;
		worker = workers[worker_id];
		printf("worker master %d \n",worker.pipMasterFd);
        ret = tyWorker_loop( worker_id);
        exit(ret);
    }
    //parent,add to writer
    else
    {
    		printf("worker child pid %d \n",pid);
        return pid;
    }
}
