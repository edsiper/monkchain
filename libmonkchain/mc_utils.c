/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  MonkChain Cli
 *  =============
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

void mc_utils_hash_to_string(unsigned char *hash, char *out)
{
    int i;
    int off;
    int out_size = 65;

    off = 0;
    for (i = 0; i < 32; i++) {
        snprintf(out + off, out_size - off, "%02x", hash[i]);
        off += 2;
    }
    out[64] = '\0';
}

void mc_utils_hash_print(unsigned char *hash)
{
    char tmp[65];

    mc_utils_hash_to_string(hash, (char *) &tmp);
    printf("%s", tmp);
}

void mc_utils_bytes_to_human_readable_size(size_t bytes,
                                           char *out_buf, size_t size)
{
    unsigned long i;
    unsigned long u = 1024;
    static const char *__units[] = {
        "b", "K", "M", "G",
        "T", "P", "E", "Z", "Y", NULL
    };

    for (i = 0; __units[i] != NULL; i++) {
        if ((bytes / u) == 0) {
            break;
        }
        u *= 1024;
    }
    if (!i) {
        snprintf(out_buf, size, "%lu%s", (long unsigned int) bytes, __units[0]);
    }
    else {
        float fsize = (float) ((double) bytes / (u / 1024));
        snprintf(out_buf, size, "%.1f%s", fsize, __units[i]);
    }
}
