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

int PrintAttributes(int what)
{
    WORD attrs = GetAttributes();

    if (attrs == ATTRIBUTE_ERROR)
        ExitSysError();

    switch (what) {
    case 1:
        attrs = attrs & 0xf;
        break;
    case 2:
        attrs = (attrs >> 4) & 0xf;
        break;
    case 3:
        attrs = attrs & 0xff;
    }

    _tprintf((what == 3) ? _T("%02X") : _T("%X"), attrs);
    return EXIT_SUCCESS;
}

int _tmain(int argc, TCHAR* argv[])
{
    WORD oldAttrs;
    WORD newAttrs;
    TCHAR* arg;
    BOOL isConsole = TRUE;

    if (argc == 1)
        return PrintAttributes(3);

    if (argc != 2)
        ExitAppError(IDS_WRONG_ARG_COUNT);

    arg = argv[1];

    if (!_tcsicmp(arg, _T("/?")))
        return PRINT_USAGE();

    if (!_tcsicmp(arg, _T("/p")))
        return PrintAttributes(3);

    if (!_tcsicmp(arg, _T("/pb")))
        return PrintAttributes(2);

    if (!_tcsicmp(arg, _T("/pg")))
        return PrintAttributes(1);

    if ((oldAttrs = GetAttributes()) == ATTRIBUTE_ERROR)
        if (GetLastError() != ERROR_INVALID_HANDLE)
            ExitSysError();
        else
            isConsole = FALSE;

    if ((newAttrs = MergeAttributes(arg, oldAttrs)) == ATTRIBUTE_ERROR)
        ExitAppError(IDS_WRONG_INDEX_FORMAT);

    if (isConsole && !SetAttributes(newAttrs))
        ExitSysError();

    return EXIT_SUCCESS;
}
