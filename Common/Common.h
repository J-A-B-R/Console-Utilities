/* Copyright 2013 Juan Burgos. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#pragma region Common
int ToWord(TCHAR* lpszWord, WORD* lpnWord, BOOL full, int radix);

BOOL StartsWith(TCHAR* str, TCHAR* prefix);

BOOL IsVistaOrAbove();

BOOL IsElevated();
#pragma endregion

#pragma region Console
#define CONSOLE_INPUT_FILE_NAME _T("CONIN$")
#define CONSOLE_OUTPUT_FILE_NAME _T("CONOUT$")
#define CONSOLE_FILE_NAME_MAX_LENGTH 7
#define ATTRIBUTE_ERROR 0xffff

typedef enum {
    Absolute,
    PositionRelative,
    LeftTopRelative,
    RightBottomRelative
} cursorCoordType_t;

BOOL IsConsoleHandle(HANDLE hHandle);

WORD MergeAttributes(TCHAR* lpszNewAttrs, WORD nCurrentAttrs);

BOOL SetCursorPosition(SHORT x, SHORT y, cursorCoordType_t xRel, cursorCoordType_t yRel);

WORD GetAttributes();

BOOL SetAttributes(WORD attr);
#pragma endregion

#pragma region Error
#ifdef _DEBUG
void Alert(TCHAR* message);
#define ALERT(x) Alert(_T(x))
#define ENSURE_DEBUGGER() if (!IsDebuggerPresent() && !(GetAsyncKeyState(VK_LSHIFT) & 0x8000)) Alert(_T("Attach a debugger now!\n\n(To skip this dialog, press left shift key next time)."))
#else
#define ALERT(x)
#define ENSURE_DEBUGGER()
#endif

#define ERROR_CUSTOM_CODE_MASK 0x2A121200

#define IS_CUSTOM_ERROR_CODE(e) ((e & ERROR_CUSTOM_CODE_MASK) == ERROR_CUSTOM_CODE_MASK)

TCHAR* GetProcessName();

TCHAR* GetSystemErrorMessage(DWORD dwMessageId);

void SysError();

void ExitSysError();

void AppError(UINT nMsgId);

void ExitAppError(UINT nMsgId);
#pragma endregion

#pragma region Memory
LPVOID MemoryAlloc(size_t count, size_t size, BOOL zeroFill);

BOOL MemoryFree(LPVOID lp);

TCHAR* StringAlloc(size_t length);

TCHAR* StringAllocAndCopy(TCHAR* lpzStr);

BOOL StringFree(TCHAR* lpzStr);
#pragma endregion

#pragma region Pipes
#define ERROR_PIPE_DATA_SIZE_MISMATCH         0x2A121201
#define ERROR_PIPE_NOT_NULL_TERMINATED_STRING 0x2A121202

BOOL SendDword(HANDLE hPipe, DWORD data);

BOOL SendData(HANDLE hPipe, LPVOID lpData, DWORD dataSize);

BOOL SendString(HANDLE hPipe, TCHAR* str);

BOOL ReceiveDword(HANDLE hPipe, LPDWORD lpData);

BOOL ReceiveData(HANDLE hPipe, LPVOID* lppData, LPDWORD dataSize);

BOOL ReceiveString(HANDLE hPipe, TCHAR** lpStr);

// Redirect a standard stream to the console
int RedirectStdToConsole(DWORD nStdHandle);
#pragma endregion

#pragma region Resources
#define PRINT_USAGE(ver, auth) (_tprintf(_T("\n%s v%s  %s\n\n"), GetProcessName(), _T(ver), _T(auth)), PrintUsage())

TCHAR* SkipFirstCmdLineArg(TCHAR* lpszCommandLine, BOOL nCorrectExtraWhiteSpace);

TCHAR* GetString(UINT nMsgId, TCHAR* lpszDefaultMsg);

int PrintUsage();
#pragma endregion

#ifdef __cplusplus
}
#endif
