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

///
/// @file
/// @copyright All code copyright Movidius Ltd 2012, all rights reserved
///            For License Warranty see: common/license.txt
///
/// @brief     Basic macros
///

#ifndef MVMACROS_H__
#define MVMACROS_H__

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((!(sizeof(x) % sizeof(0[x])))))
#ifndef MIN
#define MIN(a,b)                                \
    ({ __typeof__ (a) _a = (a);                 \
        __typeof__ (b) _b = (b);                \
        _a < _b ? _a : _b; })
#endif
#ifndef MAX
#define MAX(a,b)                                \
    ({ __typeof__ (a) _a = (a);                 \
        __typeof__ (b) _b = (b);                \
        _a > _b ? _a : _b; })
#endif
/// @brief Aligns a pointer or number to a power of 2 value given
/// @param[in] x number or pointer to be aligned
/// @param[in] a value to align to (must be power of 2)
/// @returns the aligned value
#if (defined(_WIN32) || defined(_WIN64) )
#define ALIGN_UP_UINT32(x, a)   ((uint32_t)(((uint32_t)(x) + a - 1) & (~(a-1))))
#define ALIGN_UP_INT32(x, a)   ((int32_t)(((uint32_t)(x) + a - 1) & (~(a-1))))
#define ALIGN_UP(x, a) ALIGN_UP_UINT32(x,a)
#else
#define ALIGN_UP(x, a)   ((__typeof__(x))(((uint32_t)(x) + a - 1) & (~(a-1))))
#define ALIGN_DOWN(x, a) ((__typeof__(x))(((uint32_t)(x)) & (~(a-1))) )
#define ALIGN_UP_UINT32(_x, _a)   ALIGN_UP(_x, _a)
#define ALIGN_UP_INT32(_x, _a)   ALIGN_UP(_x, _a)
#endif
/// @brief Aligns a integernumber to any value given
/// @param[in] x integer number to be aligned
/// @param[in] a value to align to
/// @returns the aligned value
#ifndef ROUND_UP
#define ROUND_UP(x, a)   ((__typeof__(x))((((uint32_t)(x) + a - 1) / a) * a))
#endif
#define ROUND_DOWN(x, a) ((__typeof__(x))(((uint32_t)(x) / a + 0) * a))

#if defined(__GNUC__) || defined(__sparc_v8__)
#define ATTR_UNUSED __attribute__((unused))
#else
#define ATTR_UNUSED
#endif

#endif

