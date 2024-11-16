//信号模块实现
#include<unistd.h>
#include<sys/syscall.h>
#include<signal.h>
#include<stdio.h>
#include "signals.h"

// 初始化信号
int initSignals(void){
    printf("%d.%ld > 忽略大部分信号\n", getpid(), syscall(SYS_gettid));
    for(int signum = SIGHUP; signum <= SIGRTMAX; signum++){
        // 保留两个信号，具体信号可以用 kill -l进行查看
        // 中间有不能进行屏蔽的信号，会报错
        if(signum != SIGINT && signum != SIGTERM){
            signal(signum, SIG_IGN);
        }
    }
    return 0;
}