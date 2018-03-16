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
#include <pwd.h>
#include <sys/stat.h>

#include <monkchain/mc_os.h>
#include <monkchain/mc_blockchain.h>

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

int mc_env_create(char *root)
{
    int ret;
    int len;
    char parent_hash[32];
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

    mc_block_genesis_parent((unsigned char *) &parent_hash);
    block = mc_block_create(root, (unsigned char *) &parent_hash);
    mc_block_save(block, root);

    return ret;
}
