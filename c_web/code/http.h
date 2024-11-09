// http模块头文件
# ifndef _HTTP_H_
# define _HTTP_H_

# include <limits.h>
# include <sys/types.h>

// 用来表示解析得到的数据
typedef struct httpRequest{
    char method[32];
    char path[PATH_MAX + 1];
    char protocol[32];
    char connection[32];
} HTTP_REQUEST;

// http 请求的解析
int parseRequest(const chat* req, HTTP_REQUEST* hreq);

// 构造响应头时需要的数据
typedef struct httpRespond{
    char protocol[32];  // 协议版本
    int status;  // 状态码
    char dest[128]; // 状态描述
    char type[64];  //  类型
    off_t length;  // 长度
    char connection[32];  // 连接状态
} HTTP_RESPOND;

// http 响应的构造 head 输出型参数
int constructHead(const HTTP_RESPOND* hres, char* head);

# endif  // __HTTP_H_