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

// 接受客户端的连接请求
int acceptClient(void){
    printf("%d.%ld > 等待客户端的连接\n", getpid(), syscall(SYS_gettid));
    struct sockaddr_in cli; // 用来输出客户端的地址结构
    socklen_t len = sizeof(cli); // 用来输出地址结构大小
    int conn = accept(s_sock, (struct sockaddr*)&cli, &len);
    if(conn == -1){
        perror("accept");
        return -1;
    }
    printf("%d.%ld > 接收到客户端%s:%hu的连接\n", getpid(), syscall(SYS_gettid),
            inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
    return conn;
}

// 接受http请求
char* recvRequest(int conn){
    char* req = NULL;  // 记录动态分配的存储区首地址
    ssize_t len = 0;  // 记录已经接受的总字节数
    for(;;){
        char buf[1024] = {};
        ssize_t size = recv(conn, buf, sizeof(buf)-1, 0);
        if(size == -1){
            perror("recv");
            free(req);
            return NULL;
        }
        if(size==0){
            printf("%d.%ld > 客户端关闭连接\n", getpid(), syscall(SYS_gettid));
            free(req);
            return NULL;
        }

        req = realloc(req, len + size + 1);  // 扩大存储区
        memcpy(req + len, buf, size + 1); // 拷贝此次接受数据到存储区
        len = len + size;  // 总长度累加

        // 如果请求中有两个\r\n, 代表接受完成
        if(strstr(req, "\r\n\r\n")){
            break;
        }
    }
}


// 发送 http 响应头
int sendHead(int conn, const char* head){
    if(send(conn, head, strlen(head), 0) == -1){
        perror("send");
        return -1;
    }
    return 0;
}

// 发送http响应体
int sendBody(int conn, const char* path){
    // 打开文件
    int fd = open(path, O_RDONLY);
    if(fd == -1){
        perror("open");
        return -1;
    }
    // 循环读取并发送
    char buf[1024];
    ssize_t len;
    // 循环结束，len-->0, 正常读完文件， len-->1,读取出错
    while((len = read(fd, buf, sizeof(buf))) > 0){
        // 接多少，发多少
        if(send(conn, buf, len, 0) == -1){
            perror("send");
            return -1;
        }
    }
    if(len == -1){
        perror("read");
        return -1;
    }
    close(fd);
    return 0;
}

// 终结套接字
void deinitSocket(void){
    close(s_sock);
}