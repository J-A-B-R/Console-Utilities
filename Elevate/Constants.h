#pragma once

#ifdef _DEBUG
#define STUB_NAME _T("_Elev.exe")
#else
#define STUB_NAME _T("Elev.exe")
#endif

#define MAX_WAITING_TIME 25000
#define RANDOM_NAME_STRZ_LENGTH 39
#define PIPE_NAME_PREFIX_LENGTH 17
#define PIPE_NAME_STRZ_LENGTH 56
#define PIPE_NAME_PREFIX _T("\\\\.\\pipe\\Elevate_")
#define PIPE_BUFFER_SIZE 1024
#define PIPE_DEFAULT_TIME_OUT 2000
