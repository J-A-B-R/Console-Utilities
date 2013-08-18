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


TCHAR* gCmdLine;
TCHAR* gCurrentDirectory;

HANDLE gIpcPipe;
TCHAR gIpcPipeName[PIPE_NAME_LENGTH + 1];

HANDLE gStdInFile;
HANDLE gStdOutFile;
HANDLE gStdErrFile;
TCHAR* gStdInFileName;
TCHAR* gStdOutFileName;
TCHAR* gStdErrFileName;

LPVOID gEnvironmentBlock;
DWORD gEnvironmentBlockSize;

DWORD gLauncherProcessId;

DWORD gTargetProcessId;
HANDLE gTargetProcess;
HANDLE gTargetThread;


void SYS_ERROR();
void APP_ERROR(DWORD nMsgId);

void SetUp();
void CleanUp();
