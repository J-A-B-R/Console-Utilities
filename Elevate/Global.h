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

#pragma once

#include "Constants.h"


#define SYS_ERROR() { SysError(); exit(CleanUp(EXIT_FAILURE)); }
#define APP_ERROR(id) { AppError(id); exit(CleanUp(EXIT_FAILURE)); }

volatile BOOL gExiting;

TCHAR* gCmdLine;
TCHAR* gCurrentDirectory;

HANDLE gIpcPipe;
TCHAR gIpcPipeName[PIPE_NAME_LENGTH + 1];

volatile HANDLE gStdInPipe;
volatile HANDLE gStdOutPipe;
volatile HANDLE gStdErrPipe;

TCHAR gStdInPipeName[PIPE_NAME_LENGTH + 1];
TCHAR gStdOutPipeName[PIPE_NAME_LENGTH + 1];
TCHAR gStdErrPipeName[PIPE_NAME_LENGTH + 1];

HANDLE gStdInThread;
HANDLE gStdOutThread;
HANDLE gStdErrThread;

HANDLE gStubProcess;
HANDLE gTargetProcess;


void SetUp();
int CleanUp(int exitCode);
