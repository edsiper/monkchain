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

#ifndef MC_BLOCKCHAIN_H
#define MC_BLOCKCHAIN_H

#define MC_MAGIC_NUMBER  0xEE004B4D
#define MC_VERSION       1

#include <monkchain/monkchain.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct mc_block {
    /* Block Header */
    uint32_t magic_number;
    uint32_t version;
    uint32_t block_id;
    unsigned char prev_hash[32];
    uint32_t timestamp;

    uint32_t block_size;

    unsigned char hash[32];
    //unsigned char merkle_root[32];
};


/* Informational blocks as a linked list */
struct mc_block_info {
    char *path;
    struct stat st;
    struct mc_block block;
    struct mk_list _head;
};

int mc_env_default(char *path, int size);
int mc_create_environment(char *root);
struct mc_block *mc_block_create(char *root);
struct mk_list *mc_block_list_create(char *root, int *count);
int mc_block_list_destroy(struct mk_list *list);

#endif
