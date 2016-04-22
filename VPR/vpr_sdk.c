#include "vpr_sdk.h"

void *handle = NULL;


DLL_VPR_Init 			VPR_Init;
DLL_VPR_Quit			VPR_Quit; 
DLL_VPR_Capture 		VPR_Capture;
DLL_VPR_GetVehicleInfo 	VPR_GetVehicleInfo; 
DLL_VPR_ComparePlate		VPR_ComparePlate; 
DLL_VPR_ComparePlateBin	VPR_ComparePlateBin; 
DLL_VPR_CheckStatus 		VPR_CheckStatus; 


//加载动态库
BOOL Dll_Init(char*  sofile)
{
	handle = dlopen(sofile, RTLD_NOW);
	if(handle != NULL)
	{
		VPR_Init = (DLL_VPR_Init)dlsym(handle,"VPR_Init"); 
		if(VPR_Init == NULL)
		{
			printf("not found VPR_Init\r\n");
			dlclose(handle);
			return 0;
		}
		VPR_Quit = (DLL_VPR_Quit)dlsym(handle,"VPR_Quit");
		if(VPR_Quit == NULL)
		{
			printf("not found VPR_Quit\r\n");
			dlclose(handle);
			return 0;
		}
		VPR_Capture = (DLL_VPR_Capture)dlsym(handle,"VPR_Capture");
		if(VPR_Capture == NULL)
		{
			printf("not found VPR_Capture\r\n");
			dlclose(handle);
			return 0;
		}
		VPR_GetVehicleInfo = (DLL_VPR_GetVehicleInfo)dlsym(handle,"VPR_GetVehicleInfo"); 
		if(VPR_GetVehicleInfo == NULL)
		{
			printf("not found VPR_GetVehicleInfo\r\n");
			dlclose(handle);
			return 0;
		}
		VPR_ComparePlate = (DLL_VPR_ComparePlate)dlsym(handle,"VPR_ComparePlate"); 
		if(VPR_ComparePlate == NULL)
		{
			printf("not found VPR_ComparePlate\r\n");
		}
		VPR_ComparePlateBin = (DLL_VPR_ComparePlateBin)dlsym(handle,"VPR_ComparePlateBin"); 
		if(VPR_ComparePlateBin == NULL)
		{
			printf("not found VPR_ComparePlateBin\r\n");
			dlclose(handle);
			return 0;
		}
		VPR_CheckStatus = (DLL_VPR_CheckStatus)dlsym(handle,"VPR_CheckStatus"); 
		if(VPR_CheckStatus == NULL)
		{
			printf("not found  VPR_CheckStatus\r\n");
			dlclose(handle);
			return 0;
		}		
		printf("Load <%s> succeed\r\n",sofile);
	}
	else
	{
		printf("Load <%s> failed by <%s>\r\n",sofile,dlerror());
		return 0;
	}
	return 1;
}
//释放动态库
void Dll_Free()
{
	if(handle == NULL)
		return;
	dlclose(handle);
	handle = NULL;
}
