#include "stdafx.h"
#include "Common.h"


LPVOID MemoryAlloc(size_t count, size_t size)
{
    LPVOID ptr;
    
    ptr = (TCHAR*)HeapAlloc(GetProcessHeap(), 0, count * size);

    if (ptr == NULL)
        // HeapAlloc doesn't call SetLastError of its own
        SetLastError(ERROR_OUTOFMEMORY);

    return ptr;
}

BOOL MemoryFree(LPVOID ptr)
{
    return (ptr != NULL) ? HeapFree(GetProcessHeap(), 0, ptr) : TRUE;
}

TCHAR* StringAlloc(size_t length)
{
    TCHAR* newStr = (TCHAR*)MemoryAlloc(length + 1, sizeof(TCHAR));

    if (newStr != NULL)
        *newStr = 0;

    return newStr;
}

TCHAR* StringAllocAndCopy(TCHAR* str)
{
    size_t len;
    TCHAR* newStr;
    
    if (str == NULL)
        return NULL;

    len = _tcslen(str);

    if ((newStr = StringAlloc(len)) != NULL)
        _tcscpy_s(newStr, len + 1, str);

    return newStr;
}

BOOL StringFree(TCHAR* str)
{
    return MemoryFree(str);
}
