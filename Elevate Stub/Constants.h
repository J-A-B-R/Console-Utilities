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

#define MAX_WAITING_TIME 25000
#define RANDOM_NAME_LENGTH 38
#define PIPE_NAME_PREFIX_LENGTH 17
#define PIPE_NAME_LENGTH (RANDOM_NAME_LENGTH + PIPE_NAME_PREFIX_LENGTH)
#define PIPE_NAME_PREFIX _T("\\\\.\\pipe\\Elevate_")
#define PIPE_BUFFER_SIZE 1024
#define PIPE_DEFAULT_TIME_OUT 2000
