#include <unistd.h>
#include <stdint.h>
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


#define WORKER_NUM 1
#define REACTOR_NUM 1

#define MAXLINE 5000
#define MAXLENGTH 2465792
#define SERV_PORT 3989
#define LOCAL_ADDR "0.0.0.0"
//监听队列长度
#define LISTENQ 10


#define SW_OK                  0
#define SW_ERR                -1
#define SW_AGAIN              -2
#define SW_BUSY               -3
#define SW_DONE               -4
#define SW_DECLINED           -5
#define SW_ABORT              -6


//!!!Don't modify.----------------------------------------------------------
#if __MACH__
#define SW_IPC_MAX_SIZE            2048  //MacOS
#else
#define SW_IPC_MAX_SIZE            8192  //for IPC, dgram and message-queue max size
#endif

#ifdef SW_USE_RINGBUFFER
#define SW_BUFFER_SIZE             65535
#else
#define SW_BUFFER_SIZE             (SW_IPC_MAX_SIZE - sizeof(struct _swDataHead))
#endif
//!!!End.-------------------------------------------------------------------

enum swFd_type
{
    SW_FD_TCP             = 0, //tcp socket
    SW_FD_LISTEN          = 1, //server socket
    SW_FD_CLOSE           = 2, //socket closed
    SW_FD_ERROR           = 3, //socket error
    SW_FD_UDP             = 4, //udp socket
    SW_FD_PIPE            = 5, //pipe
    SW_FD_WRITE           = 7, //fd can write
    SW_FD_TIMER           = 8, //timer fd
    SW_FD_AIO             = 9, //linux native aio
    SW_FD_SIGNAL          = 11, //signalfd
    SW_FD_DNS_RESOLVER    = 12, //dns resolver
    SW_FD_INOTIFY         = 13, //server socket
    SW_FD_USER            = 15, //SW_FD_USER or SW_FD_USER+n: for custom event
    SW_FD_STREAM_CLIENT   = 16, //swClient stream
    SW_FD_DGRAM_CLIENT    = 17, //swClient dgram
};
typedef struct _swDataHead
{
    int fd;
    uint16_t len;
    int16_t from_id;
    uint8_t type;
    uint8_t flags;
    uint16_t from_fd;
    uint16_t topipe_fd;
} swDataHead;

//typedef struct _swEvent
//{
//    int fd;
//    int16_t from_id;
//    uint8_t type;
//    swConnection *socket;
//} swEvent;

typedef struct _swEventData
{
    swDataHead info;
    char data[SW_BUFFER_SIZE];
} swEventData;


struct tyServer{
//	 swReactorThread *reactor_threads;
//	    swWorker *workers;
};
typedef struct _tyWorker{
	int pid;
	int pipWorkerFd;
	int pipMasterFd;
//	int pipeMasterWriteFd;
} tyWorker;


typedef struct _tyReactor{
	pthread_t pidt;
	int epfd;
} tyReactor;

