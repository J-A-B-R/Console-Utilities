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

WORD MergeAttributes(TCHAR* lpszNewAttrs, WORD nCurrentAttrs);

BOOL SetCursorPosition(SHORT x, SHORT y, BOOL xRel, BOOL yRel);

WORD GetAttributes();

BOOL SetAttributes(WORD attr);
#pragma endregion

#pragma region Error
#ifdef _DEBUG
void Alert(TCHAR* message);
#define ALERT(x) Alert(_T(x))
#define ENSURE_DEBUGGER() if (!IsDebuggerPresent()) Alert(_T("Attach debugger now!"))
#else
#define ALERT(x)
#define ENSURE_DEBUGGER()
#endif

#define ERROR_CUSTOM_CODE_MASK 0x2A121200

#define IS_CUSTOM_ERROR_CODE(e) ((e & ERROR_CUSTOM_CODE_MASK) == ERROR_CUSTOM_CODE_MASK)

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
TCHAR* SkipFirstCmdLineArg(TCHAR* lpszCommandLine, BOOL nCorrectExtraWhiteSpace);

TCHAR* GetString(UINT nMsgId, TCHAR* lpszDefaultMsg);

int PrintUsage();
#pragma endregion

#ifdef __cplusplus
}
#endif
