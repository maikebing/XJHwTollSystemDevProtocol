#ifndef _PUB_H
#define _PUB_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

#define MAX_PATH    260

typedef unsigned char byte;



//读取配置文件
void GetPrivateProfileString(char* chSection,char* chKey,char *chDefValue,char *chValue,char* chFileName);
//从INI文件读取整类型数据
int GetPrivateProfileInt(char* chSection,char* chKeyName,int nDefValue,char* chFileName);
//写配置文件
int WritePrivateProfileString(char *chAppName, char *chKeyName, char *chValue, char *chFileName);
//将整形写入ini配置文件
int WritePrivateProfileInt(char* chSection,char* chKeyName,int nValue,char* chFileName);
//函数功能：代码转换,从一种编码转为另一种编码
//返回值：成功-1 失败-0
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen);
//UNICODE码转为GB2312码
int u2g(char *inbuf,int inlen,char *outbuf,int outlen);
//GB2312码转为UNICODE码
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen);
int LoadIniFile(char *InputIniFileName);
int LoadFile(char *InputFileName,char *LoadInFileStr,int FilrStrLength);
char  *readItemInIniFile(char *ItemName);
#endif

