#pragma once

#include "Constants.h"


#define SYS_ERROR() { SysError(); CleanUp(); exit(EXIT_FAILURE); }
#define APP_ERROR(id) { AppError(id); CleanUp(); exit(EXIT_FAILURE); }

BOOL bExiting;

TCHAR* sWritableCmdLine;
TCHAR* sCurrentDirectory;

HANDLE hIpcPipe;
HANDLE hIpcThread;
TCHAR sIpcPipeName[PIPE_NAME_STRZ_LENGTH];

HANDLE hStdInPipe;
HANDLE hStdOutPipe;
HANDLE hStdErrPipe;
HANDLE hStdInThread;
HANDLE hStdOutThread;
HANDLE hStdErrThread;
TCHAR sStdInPipeName[PIPE_NAME_STRZ_LENGTH];
TCHAR sStdOutPipeName[PIPE_NAME_STRZ_LENGTH];
TCHAR sStdErrPipeName[PIPE_NAME_STRZ_LENGTH];

DWORD childProcessId;


void SetUp();
void CleanUp();
