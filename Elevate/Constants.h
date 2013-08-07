#pragma once

#ifdef _DEBUG
#define STUB_NAME _T("_Elev.exe")
#else
#define STUB_NAME _T("Elev.exe")
#endif

#define RANDOM_NAME_LENGTH 38
#define PIPE_NAME_PREFIX _T("\\\\.\\pipe\\Elevate_")
#define PIPE_NAME_PREFIX_LENGTH 17
#define PIPE_NAME_LENGTH (PIPE_NAME_PREFIX_LENGTH + RANDOM_NAME_LENGTH)
#define PIPE_BUFFER_SIZE 1024
#define PIPE_DEFAULT_TIME_OUT 2000
#define MAX_WAITING_TIME 25000
