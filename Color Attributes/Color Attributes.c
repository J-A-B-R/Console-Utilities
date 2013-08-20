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
        return PRINT_USAGE(VER_FULLVERSION, COPYRIGHT_INFO);

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
