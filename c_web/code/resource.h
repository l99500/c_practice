// 资源模块头文件
#ifndef __RESOURCE_H_
#define __RESOURCE_H_

// 判断文件是否存在
int searchResource(const char* path);

// 识别类型
int identifyType(const char* path, char* type);

#endif // __RESOURCE_H_