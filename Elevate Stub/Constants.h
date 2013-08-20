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

#define MAX_WAITING_TIME 25000
#define RANDOM_NAME_LENGTH 38
#define PIPE_NAME_PREFIX_LENGTH 17
#define PIPE_NAME_LENGTH (RANDOM_NAME_LENGTH + PIPE_NAME_PREFIX_LENGTH)
#define PIPE_NAME_PREFIX _T("\\\\.\\pipe\\Elevate_")
#define PIPE_BUFFER_SIZE 1024
#define PIPE_DEFAULT_TIME_OUT 2000
