#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <iconv.h>
#include <stdlib.h>
#include <dirent.h>
#include "pub.h"

/****************************************************************************************
* 装载Ini文件到字符串中
*****************************************************************************************/
#define MAXSYSINI 4096

char read_ini_String[MAXSYSINI];
int LoadFile(char *InputFileName,char *LoadInFileStr,int FilrStrLength)
{	
	int LoadFinleFd;
	LoadFinleFd=open(InputFileName,O_RDONLY);	
	if(LoadFinleFd<0)
	{	
		printf ("Not found file %s !!!!!!!!!!!!!!!!!!!!!!!!",InputFileName);
		return -1	;
	}
	lseek(LoadFinleFd,SEEK_SET,0);	
	if(read(LoadFinleFd,LoadInFileStr,FilrStrLength)<0)
	{	

		printf("read %s error \n",InputFileName);	
		close(LoadFinleFd);	
		return -1	;
	}
	close(LoadFinleFd);	
	return 1;
}
int LoadIniFile(char *InputIniFileName)
{	
    int ret=-1;
    ret=LoadFile(InputIniFileName,read_ini_String,MAXSYSINI);	
    return ret;

}
/****************************************************************************************
 * 在装载的Ini字符串中读取指定内容的值
 *****************************************************************************************/
char ItemValue[100];
char  *readItemInIniFile(char *ItemName)
{	
    char *ini_addr_ptr;
    int len;
     memset(ItemValue,0x00,sizeof(ItemValue));
    ini_addr_ptr = strstr(read_ini_String,ItemName);	
	if(ini_addr_ptr == NULL)
	{
		ItemValue[0] = '0';
		ItemValue[1] = 0x00;
		fprintf(stderr,"Load Config Item Error ItemName =<%s>",ItemName);
		return ItemValue;
	}
    ini_addr_ptr = strchr(ini_addr_ptr,'<')+1;
    len=strchr(ini_addr_ptr,'>')-ini_addr_ptr;
    strncpy(ItemValue,ini_addr_ptr,len);	
    ItemValue[len] = 0x00;
    return ItemValue;
}

//读取配置文件
void GetPrivateProfileString(char* chSection,char* chKey,char *chDefValue,char *chValue,char* chFileName)
{
    //保存读取的值
	char *value = "";
	//配置项
	char chSect[30];
	//保存一个字符
	char c;
	//保存一行数据
	unsigned char chLinebuf[1024];
	//打开配置文件的文件指针
	FILE *iniFp;
	//中间量
    char *p;
    //保存配置项
    sprintf(chSect,"[%s]",chSection);
	//打开文件
	iniFp=fopen(chFileName,"rb");
	//打开失败，返回默认值
	if(iniFp==NULL)
    {
        sprintf(chValue,chDefValue);
		return;
    }
	//由文件中读取一个字符
	while((c=fgetc(iniFp))!=EOF)
	{
	    //判断是否为配置项开头字符'['
		if(c!='[')
		{
            //将当前读取的字符写回文件流
            ungetc(c,iniFp);
            //由文件中读取一个字符串
            fgets((char *)(&chLinebuf[0]),1024,iniFp);
            continue;
		}
        //将制定字符写回文件流
        ungetc(c,iniFp);
        //由文件中读取一个字符串
        fgets((char *)(&chLinebuf[0]),1024,iniFp);
        //判断当前字符是否为配置文件的‘[Section]'
        if(strstr((char *)(&chLinebuf[0]),chSect) == 0)
        {
            //未找到指定的section,继续查找
            continue;
        }
        while((c=fgetc(iniFp))!='[' && c!=EOF)
        {
            //将当前读取的字符写回文件流
            ungetc(c,iniFp);
            //读取一行字符串
            fgets((char *)(&chLinebuf[0]),1024,iniFp);
            //注释行，继续查找
            if(chLinebuf[0] == ';')
                continue;
            //判断是否为读取的项
            if(strstr((char *)(&chLinebuf[0]),chKey) == 0)
                continue;
            //获取'='第一次出现的地址
            value=strchr((char *)(&chLinebuf[0]),'=');
            if(value == NULL)
            {
                sprintf(chValue,chDefValue);
                //关闭配置文件
                fclose(iniFp);
                iniFp = NULL;
                return;
            }
            //找到值的地址
            value++;
            //关闭配置文件
            fclose(iniFp);
            iniFp = NULL;
            if(*value=='\n')
            {
                sprintf(chValue,chDefValue);
                return;
            }
            sprintf(chValue,value);
            //去掉换行符
            p = strchr(chValue,'\n');
            if(p != NULL)
                *p = 0x00;
            return;
        }
        //到了文件尾部，直接返回。
        if(c==EOF)
            break;
        //将读取的字符写回文件流
        ungetc(c,iniFp);
	}
    //关闭配置文件
	fclose(iniFp);
	iniFp = NULL;
	//返回默认值
    sprintf(chValue,chDefValue);
    return;
}

//从INI文件读取整类型数据
int GetPrivateProfileInt(char* chSection,char* chKeyName,int nDefValue,char* chFileName)
{
	char chValue[50];
	GetPrivateProfileString(chSection,chKeyName,"",chValue,chFileName);
	if(strcmp(chValue,"") != 0)
		return atoi(chValue);
	else
		return nDefValue;
}
//将字串转写入ini配置文件
int WritePrivateProfileString(char *chAppName, char *chKeyName, char *chValue, char *chFileName)
{
    //文件操作指针
    FILE *fp = NULL;
    //临时指针
    char *p = NULL;
    //保存一个字符
    char c;
    //配置节点
    char chSection[100] = {0};
    //配置键
    char chKey[100] = {0};
    //中间量
    char chTemp[260] = {0};
    //保存读取每行的内容
    unsigned char chLine[1024];
    //保存配置文件内容
    unsigned char *pContent = NULL;
    //缓冲区索引
    unsigned int nIndex = 0;
    //查找结果标志 0-未找到节点 1-找到节点未找到键 2-找到键
    int nFindRet = 0;
    //中间量
    int nLen = 0;
    //组织配置域
    sprintf(chSection, "[%s]", chAppName);
    //组织配置项
    sprintf(chKey , "%s=" , chKeyName);
    //以只读方式，打开配置文件，判断文件是否存在
    fp = fopen(chFileName,"r");
    if(fp == NULL)
    {
        //以只写方式打开配置文件，若文件不存在则创建该文件
        fp = fopen(chFileName,"w");
        //文件打开失败，直接返回
        if (fp == NULL)
            return 0;
        //组织域
        sprintf(chTemp, "%s\n%s%s\n", chSection, chKey, chValue);
        //写文件
        fwrite(chTemp, strlen(chTemp),1,fp);
        //关闭文件
        fclose(fp);
        return 1;
    }
    //分配文件缓冲区
    pContent = (unsigned char *)malloc(1024*1024);
    if(pContent == NULL)
    {
        //关闭文件
        fclose(fp);
        fp = NULL;
        return 0;
    }
    //初始化文件内容缓冲区
    memset(pContent,0x00,nLen + 100);
    while((c=fgetc(fp))!=EOF)
    {
  	    //判断是否为配置项开头字符'['
		if(c!='[')
		{
            //将当前读取的字符写回文件流
            ungetc(c,fp);
            //由文件中读取一个字符串
            fgets((char *)(&chLine[0]),1024,fp);
            continue;
		}
        //将制定字符写回文件流
        ungetc(c,fp);
        //由文件中读取一个字符串
        fgets((char *)(&chLine[0]),1024,fp);
        //判断当前字符是否为配置文件的‘[Section]'
        if(strstr((char *)(&chLine[0]),chSection) == 0)
        {
            //未找到指定的section,继续查找
            continue;
        }
        //置找到节标志
        nFindRet = 1;
        //保存节点
        nLen = strlen((char *)(&chLine[0]));
        memcpy((char *)(pContent + nIndex),(char *)(&chLine[0]),nLen);
        nIndex = nIndex + nLen;
        while((c=fgetc(fp))!='[' && c!=EOF)
        {
            //将当前读取的字符写回文件流
            ungetc(c,fp);
            //读取一行字符串
            fgets((char *)(&chLine[0]),1024,fp);
            //保存读取到的内容
            if(strncmp((char *)(&chLine[0]),chKey,strlen(chKey)) != 0)
            {
                //保存节点
                nLen = strlen((char *)(&chLine[0]));
                memcpy((char *)(pContent + nIndex),(char *)(&chLine[0]),nLen);
                nIndex = nIndex + nLen;
                continue;
            }
            //置找到键标志
            nFindRet = 2;
            //保存设置的键值
            nLen = strlen(chKey) + strlen(chValue);
            sprintf((char *)(pContent + nIndex),"%s%s\n",chKey,chValue);
            nIndex = nIndex + nLen + 1;
        }
        if(nFindRet != 2)
        {
            //保存设置的键值
            nLen = strlen(chKey) + strlen(chValue);
            sprintf((char *)(pContent + nIndex),"%s%s\n",chKey,chValue);
            nIndex = nIndex + nLen + 1;
        }
        //关闭文件
        fclose(fp);
        fp = NULL;
        //以只写方式打开配置文件，若文件不存在则创建该文件
        fp = fopen(chFileName,"w");
        //文件打开失败，直接返回
        if (fp == NULL)
        {
            free(pContent);
            pContent = NULL;
            return 0;
        }
        //写文件
        fwrite((char *)pContent,nIndex,1,fp);
        //关闭文件
        fclose(fp);
        fp = NULL;
        return 1;
    }
    //关闭文件
    fclose(fp);
    fp = NULL;
    //以只写方式打开配置文件，若文件不存在则创建该文件
    fp = fopen(chFileName,"a+");
    //文件打开失败，直接返回
    if (fp == NULL)
        return 0;
    //组织域
    sprintf(chTemp, "%s\n%s%s\n", chSection, chKey, chValue);
    //写文件
    fwrite(chTemp, strlen(chTemp),1,fp);
    //关闭文件
    fclose(fp);
    return 1;
}

//将整形写入ini配置文件
int WritePrivateProfileInt(char* chSection,char* chKeyName,int nValue,char* chFileName)
{
    //中间量
	char chValue[50];
	//将整型转化成字符型
    sprintf(chValue,"%d",nValue);
	return WritePrivateProfileString(chSection,chKeyName,chValue,chFileName);
}
//函数功能：代码转换,从一种编码转为另一种编码
//返回值：成功-1 失败-0
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd==0)
		return 0;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,(size_t *)(&inlen),pout,(size_t *)(&outlen))==-1)
	{
        iconv_close(cd);
		return 0;
	}
	iconv_close(cd);
	return 1;
}
//UNICODE码转为GB2312码
int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
	return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
}
//GB2312码转为UNICODE码
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}
