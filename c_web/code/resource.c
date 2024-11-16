// 资源管理模块实现
#include<unistd.h>
#include<sys/syscall.h>
#include<string.h>
#include<stdio.h>
#include "resource.h"
#include "mime.h"

// 判断文件是否存在
int searchResource(const char* path){
    return access(path, R_OK);
}

// 识别类型
int identifyType(const char* path, char* type){
    // /home/c/error.html
    //确定拓展名
    char* suffix = strrchr(path, ".");
    if(suffix == NULL){
        printf("%d.%ld > 无法获取拓展名\n", getpid(), syscall(SYS_gettid));
        return -1;
    }
    for(int i=0; i < sizeof(s_mime) / size(s_mime[0]); i++){
        if(strcasecmp(suffix, s_mime[i].suffix) == 0){
            strcpy(type, s_mime[i].type);
            return 0;
        }
    }
    printf("%d.%ld > 不可识别的资源类型\n", getpid(), syscall(SYS_gettid));
    return -1;
}