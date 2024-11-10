// 通信模块实现
#include<unistd.h>
#include<fcntl.h>
#include<sys/syscall.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "socket.h"

static int s_sock = -1; // 侦听套接字

//初始化套接字
int initSocket(short port){
    printf("%d %ld > 创建套接字\n", getpid(), syscall(SYS_gettid));
    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(s_sock == -1){
        perror("socket");
        return -1;
    }
    // 如果服务器因为某些异常原因终止， 需要重启，
    // 可能因为端口未释放，而导致重启失败
    // 所以要对端口进行设置，允许端口被多个套接字绑定，端口复用
    printf("%d %ld > 设置套接字\n", getpid(), syscall(SYS_gettid));

    int on = 1;
    if(setsockopt(s_sock, SOL_SOCKET, 
        SO_REUSEADDR, &on, sizeof(on)) == 1){
        perror("setSocket");
        return -1;
    }

    printf("%d %ld > 组织地址结构\n", getpid(), 
            syscall(SYS_gettid));
    struct sockaddr_in ser;
    ser.sin_family = AF_INET;
    ser.sin_port = htons(port);
    ser.sin_addr.s_addr = INADDR_ANY;

    printf("%d %ld > 启动监听\n", getpid(), syscall(SYS_gettid));
    if(listen(s_sock, 1024) == -1){
        perror("listen");
        return -1;
    }

    return 0;
}