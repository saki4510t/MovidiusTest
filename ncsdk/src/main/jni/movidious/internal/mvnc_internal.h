//
// Created by saki on 2018/11/21.
// These definitions originally came from mvnc_api.c in ncsdk repository
//

/*
*
* Copyright (c) 2017-2018 Intel Corporation. All Rights Reserved
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef MOVIDIUSTEST_MVNC_INTERNAL_H
#define MOVIDIUSTEST_MVNC_INTERNAL_H

// Graph file structure
#define HEADER_LENGTH	264
#define STAGE_LENGTH 	227
#define VERSION_OFFSET 	36
#define GRAPH_VERSION 	2
#define N_STAGES_OFFSET 240
#define FIRST_SHAVE_OFFSET 248
#define N_OUTPUTS_OFFSET (HEADER_LENGTH + 136)
#define X_OUT_STRIDE_OFFSET (HEADER_LENGTH + 172)

#define THERMAL_BUFFER_SIZE 100
#define DEBUG_BUFFER_SIZE 	120

#define MAX_OPTIMISATIONS 		40
#define OPTIMISATION_NAME_LEN 	50
#define OPTIMISATION_LIST_BUFFER_SIZE (MAX_OPTIMISATIONS * OPTIMISATION_NAME_LEN)

#define MAX_PATH_LENGTH 		255
#define STATUS_WAIT_TIMEOUT     15

#endif //MOVIDIUSTEST_MVNC_INTERNAL_H
