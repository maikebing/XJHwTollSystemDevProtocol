#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include "vpr_sdk.h"
#include "pub.h"

#define TCOTYPE_TCOCMD   "00" //命令
#define TCOTYPE_TCORET    "01" //

struct MSG_TCOCMD
{
	char TCOType[2];
	char NetNo[2];
	char PlazaNo[2];
	char LaneType;
	char LaneNo[3];
	char CMD[10];
	char Param[100];
};


#define SERV_PORT 5000
#define MAXLEN (102400*5)
int socket_svr = 0;
int nCount = 0;
int  socket_udp_main = 0;
#define IPADDRESS "182.168.70.100"
typedef void *(*pReceiveMsg)(void* argsvr);
void SendToEmrc(char *msg);
void* ReceiveMsg(void* argsvr)
{
	struct sockaddr_in servaddr, cliaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	socklen_t len = sizeof(cliaddr);
	char *chImage = (char *)malloc(MAXLEN);
	char chTwo[280];
	char chPlate[16];
	char chFile[255] = {0x00};
	time_t starttm, endtm;
	for(;;)
	{
		/* waiting for receive data */
		char mesg[4] = {0x00};
		char mm[4] = {1,2,3,4};
		printf("接收照片数据 \n");
		int n = recvfrom(*((int *)argsvr), mesg, 4, 0, (struct sockaddr *)&servaddr, &len);
		if(0 == memcmp(mesg,mm,4))
		{
			int piBinLen = 0;
			int piJpegLen = 0;
			memset(chPlate,0,16);
			//LogCDev("收到车牌");
			if(TRUE == VPR_GetVehicleInfo(chPlate,&piBinLen, (BYTE*)chTwo, &piJpegLen,(BYTE*)chImage))
			{
				printf("plate:%s\n",chPlate);
				memset(chFile,0,255);

				sprintf(chFile,"../IMAGE/TEMP/3TEMP.JPG");
				FILE *out;
				if((out = fopen(chFile,"wab"))!=NULL)
				{
					printf("jpeglen:%d\n",piJpegLen);
					fwrite(chImage,1,piJpegLen,out);
					fclose(out);
				}
				memset(chFile,0,255);
				sprintf(chFile,"../IMAGE/TEMP/3TEMP.BIN");
				if((out = fopen(chFile,"wat"))!=NULL)
				{
					//printf("open%d\n",nSpecialLen);
					fwrite(chTwo,1,piBinLen,out);
					fclose(out);
				}
				//	LogCDev("图片已保存，通知主程序%s",chPlate);
				SendToEmrc(chPlate);
				//LogCDev("主程序通知完毕%s",chPlate);
				nCount++;
			}
		}
	}
	free(chImage);
}

void SendToEmrc(char *msg)
{
	struct MSG_TCOCMD tmp_sendtoemrc;
	char tmpmsg[51] ;
	memset(tmpmsg,0x00,51);
	memset((char *)&tmp_sendtoemrc,' ',sizeof(struct MSG_TCOCMD));
	sprintf(tmpmsg,"%-50s",msg);
	strncpy(tmp_sendtoemrc.Param,tmpmsg,sizeof(tmp_sendtoemrc.Param));
	strncpy(tmp_sendtoemrc.TCOType,TCOTYPE_TCOCMD,2);
	memcpy(tmp_sendtoemrc.CMD,"PLATE",sizeof(tmp_sendtoemrc.CMD));
	write(socket_udp_main,(char*)& tmp_sendtoemrc, sizeof(struct MSG_TCOCMD));
}

int createsvr()
{
	struct sockaddr_in servaddr, cliaddr;
	socket_svr = socket(AF_INET, SOCK_DGRAM, 0); /* create a socket */
	if(socket_svr <= 0)
		return -1;
	/* init servaddr */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	/* bind address and port to socket */
	if(bind(socket_svr, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("bind error");
		return -1;
	}
	pthread_t udpID;
	int *p=(int *)malloc(sizeof(int));
	*p=socket_svr;
	pReceiveMsg receMsg = ReceiveMsg;
	int ret=pthread_create(&udpID,NULL,receMsg,(void *)p);
	if(ret!=0){
		printf("getinfo thread error\n");
		return -1;
	}
	return 0;
}


void sigalrm_fn(int  sig)
{
	VPR_Capture();
	//alarm(2);
}

int  Init_EMRCMsg(void)
{
	printf("Init loc UDP   \r\n");
	if ((socket_udp_main = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("Create UDP is  error \r\n");
		return -1;
	}
	struct sockaddr_in st_srv_addr;
	bzero(&st_srv_addr, sizeof(st_srv_addr));
	st_srv_addr.sin_family = AF_INET;
	st_srv_addr.sin_port = htons (8880);
	if(connect(socket_udp_main, (struct sockaddr *)&st_srv_addr, sizeof(st_srv_addr)) == -1)
	{
		printf("Connect 8880  is error \r\n");
		return -1;
	}
	printf("Connect 8880  is Success \r\n");
	return TRUE;
}


void Init_VPR_TEST()
{
	char ipaddress[20];
	int  vprprot;
	memset(ipaddress,0x00,20);
	if (createsvr()==-1)
	{
		printf("can't init callback , maybe prot  5000 bind error\r\n");
		exit(0);
	}
	else 
	{
		LoadIniFile("./../CONFIG/CommunctionConfig.ini");
		strcpy(ipaddress,readItemInIniFile("VPRIP"));
		vprprot =atoi(readItemInIniFile("VPRPort"));
		printf("VPRIP:<%s>  VPRPort:<%d>  \r\n",ipaddress,vprprot);
		BOOL bResult = VPR_Init(SERV_PORT,vprprot,ipaddress);
		if(TRUE == bResult)
		{
			Init_EMRCMsg();
			printf("Init  success\r\n");
			signal(SIGALRM,sigalrm_fn);
			// alarm(2);
		}
		else
		{
			printf("***************Init fail*************\r\n");
			VPR_Quit();
			exit(0);
		}
	}
}


int main() 
{
	char sofile[100];
	memset(sofile,0x00,100);
	LoadIniFile("./../CONFIG/AppFunction.ini");
	sprintf(sofile,"./libVpr_%s.so",readItemInIniFile("VPRDLL"));
	printf("rhysoft VPR  V15.0412.5393\n");
	if (Dll_Init(sofile)==0)
	{
		printf("Dll_Init is error ,exit now!!\n");
	}
	else 
	{
		Init_VPR_TEST();
		printf("after 30s,1st check vpr!\n");
		sleep(30);
		while (1)
		{
			char chStatus[255] = {0x00};
			printf("check status \n");
			if (VPR_CheckStatus(chStatus)==TRUE)
			{
				printf("status is:%s,sleep 60s.\n",chStatus);
				sleep(60);
			}
			else 
			{
				printf("will exit vpr ,after 30s. \n");
				sleep(30);
				VPR_Quit();
				break;
			}
		}
	}
	return EXIT_SUCCESS;
}


