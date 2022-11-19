#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "MediaPath.h"


static bool MediaFileExists(const char *fname)
{
	FILE *fp=fopen(fname,"rb");
	if(fp == NULL)
	{
		return false;
	}
	else
	{
		fclose(fp);
		return true;
	}
}


char* FindMediaFile(const char *fname,char *buff)
{
	char curPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, curPath);
	strcat(curPath, "\\");
	strcpy(buff, curPath);
	strcat(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	char basePath[MAX_PATH];
	GetModuleFileName(NULL, basePath, MAX_PATH);
	char* pTmp = strrchr(basePath, '\\');
	if(pTmp != NULL){
		*pTmp = 0;
		SetCurrentDirectory(basePath);
		pTmp = strrchr(basePath, '\\');
		if(pTmp != NULL){
			*pTmp = 0;
		}
		pTmp = strrchr(basePath, '\\');
		if(pTmp != NULL){
			*(pTmp + 1) = 0;
		}
	}
	// printf("base path is: %s\n", basePath);
	strcpy(buff, basePath);
	strcat(buff,"Samples\\Media\\");
	strcat(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	strcpy(buff, basePath);
	strcat(buff,"Samples\\compiler\\vc8win32\\");
	strcat(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	strcpy(buff, basePath);
	strcat(buff,"Bin\\win32\\");
	strcat(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	strcpy(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	printf("Can't find media file: %s\n", fname);

	strcpy(buff,fname);
	return buff;
}


char* GetTempFilePath(char *buff)
{
	strcpy(buff,"");
	return buff;
}
