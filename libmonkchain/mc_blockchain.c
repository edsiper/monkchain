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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <time.h>

#include <monkchain/monkchain.h>
#include <monkchain/mc_os.h>
#include <monkchain/mc_log.h>
#include <sha256/sha256.h>

void mc_block_print_info(struct mc_block_info *bi)
{
    time_t t;
    char hr_size[32];
    char str_hash[65];
    char str_parent_hash[65];
    char created[32];
    struct mc_block *block = &bi->block;
    struct tm tm;

    /* Get Human Readable size */
    mc_utils_bytes_to_human_readable_size(bi->st.st_size, hr_size, 32);

    /* Hash to printable string */
    mc_block_hash_string(block, (char *) &str_hash);
    mc_utils_hash_to_string((unsigned char *) &block->prev_hash,
                            (char *) &str_parent_hash);

    /* Creation date */
    t = block->timestamp;
    gmtime_r(&t, &tm);
    strftime(created, sizeof(created) - 1, "%Y-%m-%d %H:%M:%S", &tm);

    /* Print out the info */
    mc_info("[info]\n"
            "%sBlock Details%s\n"
            " [+] hash     =>  %s\n"
            " [+] parent   =>  %s\n"
            " [+] size     =>  %i\n"
            " [+] created  =>  %s\n"

            "\n"
            "%sFile System%s\n"
            " [+] path     =>  %s\n"
            " [+] size     =>  %i bytes (%s)"

            ,
            ANSI_BOLD, ANSI_RESET,
            str_hash,
            str_parent_hash,
            block->size,
            created,

            ANSI_BOLD, ANSI_RESET,
            bi->path,
            bi->st.st_size,
            hr_size);
}

struct mc_block *mc_block_get_latest(char *root)
{
    int c;
    struct mk_list *list;
    struct mc_block_info *bi;
    struct mc_block *block;

    list = mc_block_list_create(root, &c);
    if (!list) {
        return NULL;
    }

    bi = mk_list_entry_last(list, struct mc_block_info, _head);

    block = malloc(sizeof(struct mc_block));
    if (!block) {
        mc_errno();
        mc_block_list_destroy(list);
        return NULL;
    }

    memcpy(block, &bi->block, sizeof(struct mc_block));
    mc_block_list_destroy(list);

    return block;
}


struct mc_block *mc_block_read(char *file)
{
    size_t fr;
    struct mc_block *block;
    FILE *f;

    f = fopen(file, "r");
    if (!f) {
        mc_errno();
        return NULL;
    }

    block = malloc(sizeof(struct mc_block));
    if (!block) {
        mc_errno();
        fclose(f);
        return NULL;
    }

    /* Read block info */
    fr = fread(block, sizeof(struct mc_block), 1, f);
    if (fr != 1) {
        if (errno != 0) {
            mc_errno();
        }
        fclose(f);
        free(block);
        return NULL;
    }

    fclose(f);
    return block;
}

/* Return a linked list of blocks */
struct mk_list *mc_block_list_create(char *root, int *count)
{
    int i = 0;
    int n;
    int len;
    int ret;
    int n_blocks = 0;
    char tmp[PATH_MAX];
    struct mk_list *list;
    struct mc_block *block;
    struct mc_block_info *bi;
    struct dirent *entry;
    struct dirent **namelist;

    n = scandir(root, &namelist, NULL, alphasort);
    if (n == -1) {
        mc_errno();
        return NULL;
    }

    list = malloc(sizeof(struct mk_list));
    if (!list) {
        mc_errno();
        free(namelist);
        return NULL;
    }
    mk_list_init(list);

    for (i = 0; i < n; i++) {
        entry = namelist[i];
        if (entry->d_type != DT_REG) {
            continue;
        }

        if (strncmp(entry->d_name, "blk", 3) != 0) {
            continue;
        }

        snprintf(tmp, sizeof(tmp) - 1, "%s/%s", root, entry->d_name);
        block = mc_block_read(tmp);
        if (!block) {
            mc_error("[block list] error reading %s", tmp);
            continue;
        }

        bi = malloc(sizeof(struct mc_block_info));
        bi->path = strdup(tmp);
        stat(tmp, &bi->st);
        memcpy(&bi->block, block, sizeof(struct mc_block));
        mk_list_add(&bi->_head, list);
        n_blocks++;
    }

    free(namelist);

    if (n_blocks == 0) {
        free(list);
        return NULL;
    }

    return list;
}

int mc_block_list_destroy(struct mk_list *list)
{
    int c = 0;
    struct mk_list *tmp;
    struct mk_list *head;
    struct mc_block_info *bi;

    mk_list_foreach_safe(head, tmp, list) {
        bi = mk_list_entry(head, struct mc_block_info, _head);
        mk_list_del(&bi->_head);
        free(bi->path);
        free(bi);
        c++;
    }

    free(list);
    return c;
}

/* Generate a block hash */
void mc_block_hash(struct mc_block *block, unsigned char *hash)
{
    SHA256_CTX ctx;

    SHA256_Init(&ctx);
    SHA256_Update(&ctx, &block->version, sizeof(block->version));
    SHA256_Update(&ctx, &block->timestamp, sizeof(block->timestamp));
    SHA256_Update(&ctx, &block->block_id, sizeof(block->block_id));
    SHA256_Update(&ctx, &block->prev_hash, sizeof(block->prev_hash));
    SHA256_Update(&ctx, &block->nonce, sizeof(block->nonce));
    SHA256_Final(hash, &ctx);
}

/*
 * This function is used by the mining process to reduce the computing time
 * when generating the final hash. We aim to skip the first 4 SHA256_Update()
 * and just update the final result with a given nonce on cache_do().
  */
static void mc_block_mine_hash_cache_init(SHA256_CTX *ctx, struct mc_block *block)
{
    SHA256_Init(ctx);
    SHA256_Update(ctx, &block->version, sizeof(block->version));
    SHA256_Update(ctx, &block->timestamp, sizeof(block->timestamp));
    SHA256_Update(ctx, &block->block_id, sizeof(block->block_id));
    SHA256_Update(ctx, &block->prev_hash, sizeof(block->prev_hash));
}

static void mc_block_mine_hash_cache_do(SHA256_CTX *ctx, uint32_t nonce,
                                        unsigned char *hash)
{
    SHA256_CTX tmp;

    memcpy(&tmp, ctx, sizeof(SHA256_CTX));
    SHA256_Update(&tmp, &nonce, sizeof(nonce));
    SHA256_Final(hash, &tmp);
}

/* Mine a block */
void mc_block_mine(struct mc_block *block, int difficulty)
{
    unsigned char hash[32];
    unsigned char expect[32];
    char tmp[65];
    SHA256_CTX ctx;

    memset(&expect, 0, sizeof(expect));
    mc_block_hash(block, (unsigned char *) &hash);

    /* Initialize hash cache */
    mc_block_mine_hash_cache_init(&ctx, block);
    mc_block_mine_hash_cache_do(&ctx, block->nonce, (unsigned char *) &hash);

    /* Try to find the right hash using a different nonce */
    while (memcmp(hash, expect, difficulty) != 0) {
        block->nonce++;
        mc_block_mine_hash_cache_do(&ctx, block->nonce, (unsigned char *) &hash);
    }
}

void mc_block_hash_string(struct mc_block *block, char *str_hash)
{
    unsigned char hash[32];

    mc_block_hash(block, (unsigned char *) &hash);
    mc_utils_hash_to_string((unsigned char *) &hash, str_hash);
}

void mc_block_genesis_parent(unsigned char *hash)
{
    SHA256_CTX ctx;

    SHA256_Init(&ctx);
    SHA256_Update(&ctx, MC_GENESIS_INIT, sizeof(MC_GENESIS_INIT) - 1);
    SHA256_Final(hash, &ctx);
}

/* Save a block into the file system */
int mc_block_save(struct mc_block *block, char *root)
{
    int ret;
    size_t fw;
    char target[PATH_MAX];
    struct stat st;
    FILE *f;

    snprintf(target, sizeof(target) - 1,
             "%s/" MCHAIN_FILE_FMT, root, block->block_id);

    /* Check the if the file exists */
    ret = stat(target, &st);
    if (ret == 0) {
        mc_error("block file %s already exists", target);
        return -1;
    }

    f = fopen(target, "w");
    if (!f) {
        mc_errno();
        return -1;
    }

    /* Write block info */
    fw = fwrite(block, sizeof(struct mc_block), 1, f);
    if (fw != 1) {
        mc_errno();
        fclose(f);
        return -1;
    }

    return 0;
}

struct mc_block *mc_block_create(char *root, char *parent_hash)
{
    int i;
    int off;
    char tmp[65];
    unsigned char hash[32];
    struct mc_block *latest;
    struct mc_block *block;

    block = calloc(1, sizeof(struct mc_block));
    if (!block) {
        mc_errno();
        return NULL;
    }

    block->magic_number = MC_MAGIC_NUMBER;
    block->size = sizeof(struct mc_block) - (sizeof(uint32_t) * 2);
    block->version = MC_VERSION;
    block->nonce = 0;

    /* Get the latest block */
    latest = mc_block_get_latest(root);
    if (!latest) {
        /* Genesis block */
        block->block_id = 0;
        block->timestamp = MC_GENESIS_TS;
        memset(&block->prev_hash, '\0', sizeof(block->prev_hash));
    }
    else {
        /* fixme: prev_hash */
        block->block_id = latest->block_id + 1;
        block->timestamp = time(NULL);

        if (parent_hash) {
            memcpy(&block->prev_hash, parent_hash, sizeof(block->prev_hash));
        }
        else {
            mc_block_hash(latest, (unsigned char *) &block->prev_hash);
        }
        free(latest);
    }

    mc_block_hash(block, (unsigned char *) &hash);
    mc_block_mine(block, MCHAIN_DIFFICULTY);
    mc_block_hash(block, (unsigned char *) &hash);

    mc_utils_hash_to_string(hash, tmp);
    mc_info("[block] Generate block #%i hash=%s", block->block_id, tmp);

    return block;
}
