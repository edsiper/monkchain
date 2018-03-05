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

#include <getopt.h>
#include <monkchain/monkchain.h>

static void mc_help(int rc)
{
    printf("Usage: mc [OPTION]\n\n");
    printf("%sAvailable Options%s\n", ANSI_BOLD, ANSI_RESET);
    printf("  -i, --init[=PATH]\tinitialize environment\n");
    printf("  -l, --list\t\tlist blocks\n");
    printf("  -b, --block\t\tcreate a new block\n");
    printf("  -h, --help\t\tprint this help\n");
    printf("  -v, --version\t\tshow version number\n\n");

    exit(rc);
}

static int cmd_list_blocks()
{
    int i;
    int off;
    int ret;
    int count;
    char tmp[PATH_MAX];
    char hash[65];
    struct mk_list *head;
    struct mk_list *list;
    struct mc_block_info *bi;

    ret = mc_env_default(tmp, sizeof(tmp) - 1);
    if (ret != 0) {
        return -1;
    }

    list = mc_block_list_create(tmp, &count);
    if (!list) {
        mc_error("[list blocks] could not read environment info: %s", tmp);
        return -1;
    }

    mk_list_foreach(head, list) {
        bi = mk_list_entry(head, struct mc_block_info, _head);

        /* Hash to Hex */
        off = 0;
        for (i = 0; i < 32; i++) {
            snprintf(hash + off, sizeof(hash) - off, "%02x", bi->block.hash[i]);
            off += 2;
        }
        hash[64] = '\0';

        printf("[blk %08i] %s %s\n",
               bi->block.block_id, hash, bi->path);
    }
}

static int cmd_block_create()
{
    int i;
    int off;
    int ret;
    int count;
    char tmp[PATH_MAX];
    char hash[65];
    struct mk_list *head;
    struct mk_list *list;
    struct mc_block *block;

    ret = mc_env_default(tmp, sizeof(tmp) - 1);
    if (ret != 0) {
        return -1;
    }

    block = mc_block_create(tmp);
    if (!block) {
        return -1;
    }
    mc_block_save(block, tmp);

    free(block);
    return 0;
}

int main(int argc, char **argv)
{
    int ret;
    int opt;
    int optid = 1;

    static const struct option long_opts[] = {
        {"init",  optional_argument, NULL, 'i'},
        {"block", no_argument      , NULL, 'b'},
        {"list",  no_argument      , NULL, 'l'},
        {"help",  no_argument      , NULL, 'h'},
    };

    while ((opt = getopt_long(argc, argv, "i::blh", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'i':
            ret = mc_create_environment(optarg);
            return ret;
        case 'b':
            ret = cmd_block_create();
            return ret;
        case 'l':
            ret = cmd_list_blocks();
            return ret;
        case 'h':
            mc_help(0);
            break;
        default:
            mc_help(EXIT_FAILURE);
        }
    }

    mc_help(EXIT_FAILURE);

    return 0;
}
