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
#include "Global.h"
#include "Process.h"


int _tmain(int argc, TCHAR* argv[])
{
    int result;

    ENSURE_DEBUGGER();

    if (argc == 1 || (argc == 2 && !_tcsicmp(*(argv + 1), _T("/?"))))
        return PRINT_USAGE(VER_FULLVERSION, COPYRIGHT_INFO);

    // TODO:
    // /r (raw) do not interpret internal cmd commands as cmd /d /c command
    // /n do not copy env vars

    SetUp();
    result = (!IsVistaOrAbove() || IsElevated()) ? ExecuteProgram() : ExecuteStub();
    return CleanUp(result);
}
