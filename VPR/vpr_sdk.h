#ifndef _VPR_SDK_H_
#define _VPR_SDK_H_

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 


typedef enum{TRUE=1,FALSE=!TRUE} BOOL;
typedef unsigned int UINT;
typedef char BYTE;


typedef BOOL (*DLL_VPR_Init)(UINT uPort,int nHWYPort,char *chDevIp);
typedef BOOL (*DLL_VPR_Quit)();
typedef BOOL (*DLL_VPR_Capture)();
typedef BOOL (*DLL_VPR_GetVehicleInfo)(char *pchPlate,int *piByteBinImagLen,BYTE *pByteBinImage,int *piJpegImageLen,BYTE *pByteJpegImage);
typedef int (*DLL_VPR_ComparePlate)(int iFullCompare,int iRule,char *chPlateIn,char *chPlateOut);
typedef BOOL (*DLL_VPR_ComparePlateBin)(BYTE *lpBinImageIn,BYTE *lpBinImageOut);
typedef BOOL (*DLL_VPR_CheckStatus)(char *chVprDevStatus);



extern DLL_VPR_Init 			VPR_Init;
extern DLL_VPR_Quit			VPR_Quit; 
extern DLL_VPR_Capture 		VPR_Capture;
extern DLL_VPR_GetVehicleInfo 	VPR_GetVehicleInfo; 
extern DLL_VPR_ComparePlate		VPR_ComparePlate; 
extern DLL_VPR_ComparePlateBin	VPR_ComparePlateBin; 
extern DLL_VPR_CheckStatus 		VPR_CheckStatus; 


//加载动态库
BOOL Dll_Init(char* sofile);

//释放动态库
void Dll_Free();


#endif
