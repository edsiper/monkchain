/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  MonkChain
 *  =========
 *  Copyright (C) 2018 Eduardo Silva <eduardo@monkey.io>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef MC_LOG_H
#define MC_LOG_H

/* Message types */
#define MC_LOG_OFF     0
#define MC_LOG_ERROR   1
#define MC_LOG_WARN    2
#define MC_LOG_INFO    3  /* default */
#define MC_LOG_DEBUG   4
#define MC_LOG_TRACE   5

static inline int mc_log_check(int l) {
    (void) l;
    return MC_TRUE;
}


/* Logging macros */
#define mc_error(fmt, ...)                                          \
    if (mc_log_check(MC_LOG_ERROR))                                 \
        mc_log_print(MC_LOG_ERROR, NULL, 0, fmt, ##__VA_ARGS__)

#define mc_warn(fmt, ...)                                           \
    if (mc_log_check(MC_LOG_WARN))                                  \
        mc_log_print(MC_LOG_WARN, NULL, 0, fmt, ##__VA_ARGS__)

#define mc_info(fmt, ...)                                           \
    if (mc_log_check(MC_LOG_INFO))                                  \
        mc_log_print(MC_LOG_INFO, NULL, 0, fmt, ##__VA_ARGS__)

#define mc_debug(fmt, ...)                                         \
    if (mc_log_check(MC_LOG_DEBUG))                                \
        mc_log_print(MC_LOG_DEBUG, NULL, 0, fmt, ##__VA_ARGS__)


#ifdef __FILENAME__
#define mc_errno() mc_errno_print(errno, __FILENAME__, __LINE__)
#else
#define mc_errno() mc_errno_print(errno, __FILE__, __LINE__)
#endif

int mc_errno_print(int errnum, const char *file, int line);
void mc_log_print(int type, const char *file, int line, const char *fmt, ...);

#endif
