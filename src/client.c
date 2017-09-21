/**
 * 客户端程序
 * 1.创建socket
 * 2.建立连接
 * 3.发送数据
 * 4.接受数据
 * 5.关闭连接
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdbool.h>

int cl_socket;
struct sockaddr_in serv_addr;
char recvBuff[1024];
//创建socket
int createSocket() {
	cl_socket = 0;
	int n = 0;
	char recvBuff[1024];
	if ((cl_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Error : Could not create socket \n");
		return 1;
	}
	return cl_socket;
}
//连接服务器
int cli_connect(char* ip, int port) {
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
		printf("\n inet_pton error occured\n");
		return 1;
	}
	if (connect(cl_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
		printf("\n Error : Connect Failed \n");
		return 1;
	}
	return 0;
}
//发送数据
int cli_send(char* data, int dataLength) {
	int length = write(cl_socket, data, dataLength);
	printf("length %d",dataLength);
	printf("cl_socket %d",cl_socket);
	return length;
}
//接受数据
char *cli_recv() {
	int n = 0;
	while ((n = read(cl_socket, recvBuff, sizeof(recvBuff) - 1)) > 0) {
		recvBuff[n] = 0;
		if (fputs(recvBuff, stdout) == EOF) {
			printf("\n Error : Fputs error\n");
		}
	}
	return recvBuff;
}
//关闭连接
void cli_close() {
	close(cl_socket);
}
