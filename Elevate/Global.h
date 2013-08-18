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
