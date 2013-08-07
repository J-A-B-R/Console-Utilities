#pragma once

#include "Constants.h"


#define SYS_ERROR() { SysError(); exit(CleanUp(EXIT_FAILURE)); }
#define APP_ERROR(id) { AppError(id); exit(CleanUp(EXIT_FAILURE)); }

volatile BOOL gExiting;

TCHAR* gCmdLine;
TCHAR* gCurrentDirectory;

HANDLE gIpcPipe;
TCHAR gIpcPipeName[PIPE_NAME_LENGTH + 1];

volatile HANDLE gStdInPipe;
volatile HANDLE gStdOutPipe;
volatile HANDLE gStdErrPipe;

TCHAR gStdInPipeName[PIPE_NAME_LENGTH + 1];
TCHAR gStdOutPipeName[PIPE_NAME_LENGTH + 1];
TCHAR gStdErrPipeName[PIPE_NAME_LENGTH + 1];

HANDLE gStdInThread;
HANDLE gStdOutThread;
HANDLE gStdErrThread;

HANDLE gStubProcess;
HANDLE gTargetProcess;


void SetUp();
int CleanUp(int exitCode);
