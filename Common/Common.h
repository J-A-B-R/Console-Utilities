#pragma once

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define ATTRIBUTE_ERROR 0xffff

#ifdef _DEBUG
void Alert(TCHAR* message);
#define ALERT(x) Alert(_T(x))
#else
#define ALERT(x)
#endif

#ifdef __cplusplus
extern "C" {
#endif

LPVOID MemoryAlloc(size_t count, size_t size);

BOOL MemoryFree(LPVOID lp);

TCHAR* StringAlloc(size_t length);

TCHAR* StringAllocAndCopy(TCHAR* lpzStr);

BOOL StringFree(TCHAR* lpzStr);

TCHAR* SkipFirstCmdLineArg(TCHAR* lpszCommandLine, BOOL nCorrectExtraWhiteSpace);

TCHAR* GetString(UINT nMsgId, TCHAR* lpszDefaultMsg);

int PrintUsage();

void SysError();

void ExitSysError();

void AppError(UINT nMsgId);

void ExitAppError(UINT nMsgId);

BOOL IsVistaOrAbove();

BOOL IsElevated();

// Create a child process that uses the specified standard handles.
BOOL CreateChildProcess(HANDLE stdIn,
                        HANDLE stdOut,
                        HANDLE stdErr,
                        TCHAR* szCmdLine,
                        LPVOID lpEnvironment,
                        LPTSTR lpCurrentDirectory,
                        PROCESS_INFORMATION* piProcInfo);

int ToWord(TCHAR* lpszWord, WORD* lpnWord, BOOL full, int radix);

WORD MergeAttributes(TCHAR* lpszNewAttrs, WORD nCurrentAttrs);

BOOL SetCursorPosition(SHORT x, SHORT y, BOOL xRel, BOOL yRel);

WORD GetAttributes();

BOOL SetAttributes(WORD attr);

// Redirect a standard stream to the console
int RedirectStdToConsole(DWORD nStdHandle);

#ifdef __cplusplus
}
#endif
