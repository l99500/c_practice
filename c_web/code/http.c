#include <unistd.h>  // 操作进程和文件
#include <sys/syscall.h> // 打印系统内核的线程id
#include <stdio.h>
#define __USE_GNU // 定义之后 string.h 可以使用 strcasestr 字母大小写不敏感
#include <string.h>
#include <time.h>
#include http.h

// http 请求的解析
int parseRequest(const chat* req, HTTP_REQUEST* hreq){
    sscanf(req, "%s%s%s", hreq->method, hreq->path, hreq->protocol);
    char* connection = strcasestr(req, "connection");
    if (connection){
        sscanf(connection, "%*S%s", hreq->connection);  // %*S 虚读 跳过不读
    }
    printf("%d.%ld > [%s][%s][%s][%s]\n", 
            getpid(),    // 进程内部的线程id
            syscall(SYS_gettid),  // 系统内部的线程id
            hreq->method,
            hreq->path,
            hreq->protocol,
            hreq->connection);
    // 判断请求方法
    if (strcasecmp(hreq->method, "get")){
        printf("%d.%ld > 无效的方法\n", getpid(), syscall(SYS_gettid));
        return -1;
    }
    // 判断协议版本
    if (strcasecmp(hreq->protocol, "http/1.0") &&
        strcasecmp(hreq->protocol, "http/1.1")){
        printf("%d.%ld > 无效的协议\n", getpid(), syscall(SYS_gettid));
        return -1;
     }
    return 0;
}

// http响应的构造
int constructHead(const HTTP_RESPOND* hres, char* head){
    char dateTime[32];
    time_t now = time(NULL);

    return 0;
}