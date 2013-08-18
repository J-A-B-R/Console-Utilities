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

HANDLE CreateIpcPipe(TCHAR* lpPipeName);

HANDLE CreateRedirectionPipe(TCHAR* lpPipeName, DWORD nStdHandle);

HANDLE StartHandlingRedirectionPipe(HANDLE hPipe, DWORD nStdHandle);

void SignalHandlingRedirecionPipesForExiting();

BOOL WaitHandlingRedirecionPipesForExiting();
