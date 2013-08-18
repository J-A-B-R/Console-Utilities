/* Copyright (c) 2013 Juan Burgos. All rights reserved.
 *
 * This source is subject to the Apache 2.0 License.
 * Please see the included LICENSE file for more information.
 * All other rights reserved.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "stdafx.h"
#include "Common.h"


LPVOID MemoryAlloc(size_t count, size_t size, BOOL zeroFill)
{
    LPVOID ptr;
    
    ptr = (TCHAR*)HeapAlloc(GetProcessHeap(), (zeroFill) ? HEAP_ZERO_MEMORY : 0, count * size);

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
    TCHAR* newStr = (TCHAR*)MemoryAlloc(length + 1, sizeof(TCHAR), FALSE);

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
