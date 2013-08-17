#pragma once


TCHAR* gCmdLine;
TCHAR* gCurrentDirectory;

HANDLE gIpcPipe;
TCHAR gIpcPipeName[PIPE_NAME_LENGTH + 1];

HANDLE gStdInFile;
HANDLE gStdOutFile;
HANDLE gStdErrFile;
TCHAR* gStdInFileName;
TCHAR* gStdOutFileName;
TCHAR* gStdErrFileName;

LPVOID gEnvironmentBlock;
DWORD gEnvironmentBlockSize;

DWORD gLauncherProcessId;

DWORD gTargetProcessId;
HANDLE gTargetProcess;
HANDLE gTargetThread;


void SYS_ERROR();
void APP_ERROR(DWORD nMsgId);

void SetUp();
void CleanUp();
