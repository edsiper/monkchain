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

#include <monkchain/monkchain.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct mc_block {
    /* Data format ID */
    uint32_t magic_number;

    /* Block size excluding magic_number and this field */
    uint32_t size;

    /* Block Header */
    uint32_t version;
    uint32_t timestamp;
    uint32_t block_id;
    uint32_t nonce;
    unsigned char prev_hash[32];

    /* Merkle Root */
    unsigned char merkle_root[32];

    /* Transaction Number */

    /* Transactions */
};

/* Informational blocks as a linked list */
struct mc_block_info {
    char *path;
    struct stat st;
    struct mc_block block;
    struct mk_list _head;
};

/* Monkchain object context */
struct mc_blockchain {

};

struct mc_block *mc_block_create(char *root, char *parent_hash);
struct mk_list *mc_block_list_create(char *root, int *count);

int mc_block_save(struct mc_block *block, char *root);
void mc_block_hash(struct mc_block *block, unsigned char *hash);
void mc_block_hash_string(struct mc_block *block, char *str_hash);

int mc_block_list_destroy(struct mk_list *list);
void mc_block_print_info(struct mc_block_info *bi);
void mc_block_genesis_parent(unsigned char *hash);

#endif
