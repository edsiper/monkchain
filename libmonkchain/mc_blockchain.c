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

#include <monkchain/monkchain.h>
#include <monkchain/mc_os.h>
#include <monkchain/mc_log.h>
#include <sha256/sha256.h>

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
        mc_errno();
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
    SHA256_Update(&ctx, &block->magic_number, sizeof(block->magic_number));
    SHA256_Update(&ctx, &block->version, sizeof(block->version));
    SHA256_Update(&ctx, &block->timestamp, sizeof(block->timestamp));
    SHA256_Update(&ctx, &block->block_id, sizeof(block->block_id));
    SHA256_Update(&ctx, &block->prev_hash, sizeof(block->prev_hash));
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

struct mc_block *mc_block_create(char *root)
{
    int i;
    int off;
    char tmp[65];
    struct mc_block *latest;
    struct mc_block *block;

    block = malloc(sizeof(struct mc_block));
    if (!block) {
        mc_errno();
        return NULL;
    }

    block->magic_number = MC_MAGIC_NUMBER;
    block->version = MC_VERSION;
    block->timestamp = time(NULL);

    /* Get the latest block */
    latest = mc_block_get_latest(root);
    if (!latest) {
        memset(block->prev_hash, '\0', sizeof(block->prev_hash));
        block->block_id = 0;
    }
    else {
        /* fixme: prev_hash */
        block->block_id = latest->block_id + 1;
    }

    mc_block_hash(block, (unsigned char *) &block->hash);
    off = 0;
    for (i = 0; i < 32; i++) {
        snprintf(tmp + off, sizeof(tmp) - off, "%02x", block->hash[i]);
        off += 2;
    }
    tmp[64] = '\0';
    mc_info("[block] Generate block #%i hash=%s", block->block_id, tmp);
    return block;
}

int mc_env_default(char *path, int size)
{
    int len;
    struct passwd *pw;

    pw = getpwuid(getuid());
    if (!pw) {
        perror("getpwuid");
        return -1;
    }

    /* ~/.monkchain */
    len = snprintf(path, size, "%s/%s",
                   pw->pw_dir, MCHAIN_ENV_PATH);
    if (len == -1) {
        perror("snprintf");
        return -1;
    }

    return 0;
}

int mc_create_environment(char *root)
{
    int ret;
    int len;
    char *target;
    char tmp[PATH_MAX];
    struct stat st;
    struct mc_block *block;

    if (!root) {
        /* Create an environment based in the current user's home directory */
        mc_env_default(tmp, sizeof(tmp) - 1);
        root = tmp;
    }

    ret = stat(root, &st);
    if (ret == 0) {
        mc_error("[create env] path already exists: %s", root);
        return -1;
    }

    ret = mc_os_mkpath(root, 0755);
    if (ret == 0) {
        mc_info("[create env] path %s OK", root);
    }
    else {
        mc_error("[create env] could not create path %s", root);
        mc_errno();
        return -1;
    }

    block = mc_block_create(root);
    mc_block_save(block, root);

    return ret;
}
