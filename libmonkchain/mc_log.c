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

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include <monkchain/monkchain.h>
#include <monkchain/mc_log.h>

void mc_log_print(int type, const char *file, int line, const char *fmt, ...)
{
    int len;
    int total;
    time_t now;
    const char *header_color = NULL;
    const char *header_title = NULL;
    const char *bold_color = ANSI_BOLD;
    const char *reset_color = ANSI_RESET;
    char msg[PATH_MAX];
    struct tm result;
    struct tm *current;
    va_list args;

    va_start(args, fmt);

    switch (type) {
    case MC_LOG_INFO:
        header_title = "info";
        header_color = ANSI_GREEN;
        break;
    case MC_LOG_WARN:
        header_title = "warn";
        header_color = ANSI_YELLOW;
        break;
    case MC_LOG_ERROR:
        header_title = "error";
        header_color = ANSI_RED;
        break;
    case MC_LOG_DEBUG:
        header_title = "debug";
        header_color = ANSI_YELLOW;
        break;
    case MC_LOG_TRACE:
        header_title = "trace";
        header_color = ANSI_BLUE;
        break;
    }

    /* Only print colors to a terminal */
    if (!isatty(STDOUT_FILENO)) {
        header_color = "";
        bold_color = "";
        reset_color = "";
    }

    now = time(NULL);
    current = localtime_r(&now, &result);

    len = snprintf(msg, sizeof(msg) - 1,
                   "%s[%s%i/%02i/%02i %02i:%02i:%02i%s]%s [%s%5s%s] ",
                   /*      time     */                    /* type */

                   /* time variables */
                   bold_color, reset_color,
                   current->tm_year + 1900,
                   current->tm_mon + 1,
                   current->tm_mday,
                   current->tm_hour,
                   current->tm_min,
                   current->tm_sec,
                   bold_color, reset_color,

                   /* type format */
                   header_color, header_title, reset_color);

    total = vsnprintf(msg + len,
                      (sizeof(msg) - 2) - len,
                      fmt, args);
    if (total < 0) {
        return;
    }

    total = strlen(msg + len) + len;
    msg[total++] = '\n';
    msg[total]   = '\0';
    va_end(args);

    fprintf(stderr, "%s", (char *) msg);
}

int mc_errno_print(int errnum, const char *file, int line)
{
    char buf[256];

    strerror_r(errnum, buf, sizeof(buf) - 1);
    mc_error("[%s:%i errno=%i] %s", file, line, errnum, buf);
    return 0;
}
