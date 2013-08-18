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

// Restores the original colors before exiting with error
#define APP_ERROR(id) { if (isConsole) SetAttributes(originalAttrs); ExitAppError(id); }
#define SYS_ERROR() { if (isConsole) { SetAttributes(originalAttrs); ExitSysError(); } }

#define DEFAULT_ESCAPE_CHARACTER '@'

int _tmain(int argc, _TCHAR* argv[])
{
    TCHAR escChar = DEFAULT_ESCAPE_CHARACTER;
    int i;
    int state;
    WORD originalAttrs;
    WORD oldAttrs;
    WORD tmp;
    TCHAR* arg;
    TCHAR k;
    TCHAR buffer[5];
    TCHAR* ptr;
    BOOL xRel;
    BOOL yRel;
    __int64 x;
    __int64 y;
    BOOL isConsole = TRUE;

    if (argc == 1) {
        _tprintf(_T("\n"));
        return EXIT_SUCCESS;
    }

    if (!_tcsicmp(argv[1], _T("/?")))
        return PRINT_USAGE(VER_FULLVERSION, COPYRIGHT_INFO);

    if ((originalAttrs = oldAttrs = GetAttributes()) == ATTRIBUTE_ERROR) {
        SYS_ERROR();
        isConsole = FALSE;
    }
    
    arg = SkipFirstCmdLineArg(GetCommandLine(), TRUE);

    for (i = 0, state = 0, k = *arg; k != 0; k = *(arg + ++i)) {
        switch (state)
        {
        case 0:
            if (k == '@') {
                state = 1;
            } else {
                _puttchar(k);
            }
            break;
        case 1:
            switch (k) {
            case 'c':
                state = 10;
                break;
            case 'C':
                state = 20;
                break;
            case 'b':
                _puttchar('\b');
                state = 0;
                break;
            case 'g':
                _puttchar('\7');
                state = 0;
                break;
            case 'n':
                _puttchar('\n');
                state = 0;
                break;
            case 'r':
                _puttchar('\r');
                state = 0;
                break;
            case 't':
                _puttchar('\t');
                state = 0;
                break;
            case '@':
                _puttchar('@');
                state = 0;
                break;
            case '[':
                state = 30;
                break;
            case '=':
                state = 40;
                break;
            case '#':
                state = 0;
                goto exit_loop;
            default:
                if (k == escChar) {
                    _puttchar(escChar);
                    state = 0;
                } else {
                    APP_ERROR(IDS_UNKNOWN_OPTION);
                }
                break;
            }
            break;
        case 10:
            if (k == 0 || k == ' ' || k == '\t')
                APP_ERROR(IDS_WRONG_OPTION_ARG);

            buffer[0] = k;
            state = 11;
            break;
        case 11:
            if (k == 0 || k == ' ' || k == '\t')
                APP_ERROR(IDS_WRONG_OPTION_ARG);

            buffer[1] = k;
            buffer[2] = 0;
            if ((tmp = MergeAttributes(buffer, oldAttrs)) == ATTRIBUTE_ERROR)
                APP_ERROR(IDS_WRONG_INDEX_FORMAT);

            if (isConsole && !SetAttributes(oldAttrs = tmp))
                SYS_ERROR();

            state = 0;
            break;
        case 20:
            if (k == ';')
                APP_ERROR(IDS_WRONG_COORD_FORMAT);

            xRel = k == '+' || k == '-';
            x = _tcstoi64(arg + i, &ptr, 10);
            if (errno != 0 || x > SHRT_MAX || x < SHRT_MIN)
                APP_ERROR(IDS_WRONG_COORD_FORMAT);

            i = (int)(ptr - arg);
            if (*ptr == ';') {
                if (isConsole && !SetCursorPosition((SHORT)x, 0, xRel, TRUE))
                    SYS_ERROR();

                state = 0;
            } else if (*ptr == ',')
                state = 21;
            else
                APP_ERROR(IDS_WRONG_COORD_FORMAT);
            break;
        case 21:
            if (k == ';') {
                if (isConsole && !SetCursorPosition((SHORT)x, 0, xRel, TRUE))
                    SYS_ERROR();
            } else {
                yRel = k == '+' || k == '-';
                y = _tcstoi64(arg + i, &ptr, 10);
                if (errno != 0 || x > SHRT_MAX || x < SHRT_MIN || *ptr != ';')
                    APP_ERROR(IDS_WRONG_COORD_FORMAT);

                i = (int)(ptr - arg);
                if (isConsole && !SetCursorPosition((SHORT)x, (SHORT)y, xRel, yRel))
                    SYS_ERROR();
            }
            state = 0;
            break;
        case 30:
            if (k == '[') {
                state = 31;
            } else {
                APP_ERROR(IDS_WRONG_OPTION_ARG);
            }
            break;
        case 31:
            if (k == ']') {
                state = 32;
            } else {
                _puttchar(k);
            }
            break;
        case 32:
            if (k == ']') {
                state = 0;
            } else {
                _puttchar(']');
                _puttchar(k);
                state = 31;
            }
            break;
        case 40:
            escChar = k;
            state = 0;
            break;
        }
    }

exit_loop:
    if (isConsole && !SetAttributes(originalAttrs))
        SYS_ERROR();
    if (state != 0)
        AppError(IDS_WRONG_OPTION_ARG);
    return EXIT_SUCCESS;
}
