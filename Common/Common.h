#pragma once

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define ATTRIBUTE_ERROR 0xffff
#define MARKER_BUFFER_LENGTH 12

#ifdef _DEBUG
void Alert(TCHAR* message);
#define ALERT(x) Alert(_T(x))
#else
#define ALERT(x)
#endif

TCHAR* SkipFirstCmdArg(TCHAR* lpszCommandLine);

TCHAR* GetString(UINT nMsgId, TCHAR* lpszDefaultMsg);

int PrintUsage();

void SysError();

void AppError(UINT nMsgId);

BOOL IsVistaOrAbove();

BOOL IsElevated();

int ToWord(TCHAR* lpszWord, WORD* lpnWord, BOOL full, int radix);

WORD MergeAttributes(TCHAR* lpszNewAttrs, WORD nCurrentAttrs);

BOOL SetCursorPosition(SHORT x, SHORT y, BOOL xRel, BOOL yRel);

WORD GetAttributes();

BOOL SetAttributes(WORD attr);

// Redirect a standard stream to the console
int RedirectStdToConsole(DWORD nStdHandle);
