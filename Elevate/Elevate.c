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
#include "Global.h"
#include "Process.h"


int _tmain(int argc, TCHAR* argv[])
{
    int result;

    ENSURE_DEBUGGER();

    if (argc == 1 || (argc == 2 && !_tcsicmp(*(argv + 1), _T("/?"))))
        return PRINT_USAGE();

    // TODO:
    // /r (raw) do not interpret internal cmd commands as cmd /d /c command
    // /n do not copy env vars

    SetUp();
    result = (!IsVistaOrAbove() || IsElevated()) ? ExecuteProgram() : ExecuteStub();
    return CleanUp(result);
}
