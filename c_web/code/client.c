// 线程模块实现
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "http.h"
#include "socket.h"
#include "resource.h"
#include "client.h"

// 线程过程函数
void* client(void* arg){
    CA* ca = (CA*)arg;
    printf("%d.%ld > 客户机线程处理开始\n", getpid(), syscall(SYS_gettid));
    for(;;){
        printf("%d.%ld > 接受http请求\n", getpid(), syscall(SYS_gettid));
        char* req = recvRequest(ca->conn);
        if(req == NULL){
            break;
        }
        printf("%d.%ld > 请求电文：\n%s\n", getpid(), syscall(SYS_gettid),req);
        //解析请求
        printf("%d.%ld > 解析请求：\n%s\n", getpid(), syscall(SYS_gettid),req);
        HTTP_REQUEST hreq = {}; // 用来输出请求中的关键数据
        if(parseRequest(req, &hreq) == -1){
            free(req); // 解析失败，释放存储区
            req = NULL;
            break;
        }
        free(req);  // 解析成功释放存储区，后续数据使用hreq结构体即可
        req = NULL;

        //路径处理
        // /c/error.html 请求路径
        // /home/tarena/2208/project/home 存储路径
        // /home/tarena/2208/project/home/c/error.html  /真实路径
        char root[PATH_MAX + 1];  // 存储路径
        char path[PATH_MAX + 1]; // 真实路径
        // 去除路径中最后的/
        if(root[strlen(root) - 1] == '/'){
            root[strlen(root) - 1] = '\0';
        }
        //拼接文件的真实路径
        strcpy(path, root);
        strcat(path, hreq.path);

        // 如果请求路径是 “/”，则应将首页文件响应给对方
        if(strcmp(hreq.path, "/") == 0){
            // /home/tarena/2208/project/home/index.html
            strcat(path, "index.html");
        }

        // 构造响应时所属的结构体
        HTTP_RESPOND hres = {"HTTP/1.1", 200, "OK", "text/html"};
        // 获取资源
        if(searchResource(path) == -1){
            hres.status = 404;
            strcpy(hres.desc, "NOT FOUND");
            // /home/tarena/2208/project/home/404.html
            strcpy(path, root);
            strcat(path, "/404.html");
        }else if(identifyType(path, hres.type) == -1){
            hres.status = 404;
            // /home/tarena/2208/project/home/404.html
            strcpy(path, root);
            strcat(path, "/404.html");
        }

        // 确定文本长度
        struct stat st;  // 用来输出文本的元数据
        if(stat(path, &st) == -1){
            perror("stat");
            break;
        }
        hres.length = st.st_size;

        // 连接状态
        if(strlen(hreq.connection)){
            strcpy(hres.connection, hreq.connection);
        }else if(strcasecmp(hreq.protocol, "http/1.0") == 0){
            strcpy(hres.connection, "Close");
        }else{
            strcpy(hres.connection, "Keep-alive");
        }

        // 构造响应
        printf("%d.%ld > 构造响应：\n", getpid(), syscall(SYS_gettid));
        char head[1024];
        if(constructHead(&hres, head) == -1){
            break;
        }
        printf("%d.%ld > 响应电文：\n%s\n", getpid(), syscall(SYS_gettid),head);

        // 发送响应头
        if(sendHead(ca->conn, head) == -1){
            break;
        } 

        // 发送响应体
        if(sendBody(ca->conn, path) == -1){
            break;
        }

        // 如果连接状态是close则退循环
        if(strcasecmp(hres.connection, "close") == -1){
            break;
        }
    }
    close(ca->conn);
    free(ca);
    printf("%d.%ld > 客户机线程处理结束\n", getpid(), syscall(SYS_gettid));
    return NULL;
}