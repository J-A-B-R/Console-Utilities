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
