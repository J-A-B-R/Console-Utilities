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


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    ENSURE_DEBUGGER();

    SetUp();
    CheckSuitability();
    StartIpcCommunication(lpCmdLine);
    ReceiveIpcData();
    PrepareForLaunching();
    CreateTargetProcess();
    FinishIpcCommunication();
    ResumeAndWaitForTargetProcess();
    CleanUp();

    return EXIT_SUCCESS;
}
