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

    printf("  -I, --init[=PATH]\tinitialize environment\n");
    printf("  -i, --info[=PATH]\tinitialize environment\n");
    printf("  -l, --list\t\tlist blocks\n");
    printf("  -b, --block\t\tcreate a new block\n");
    printf("  -h, --help\t\tprint this help\n");
    printf("  -v, --version\t\tshow version number\n\n");

    exit(rc);
}

static int cmd_list_blocks(char *env_path)
{
    int i;
    int off;
    int ret;
    int count;
    char tmp[PATH_MAX];
    unsigned char hash[32];
    char str_hash[65];
    struct mk_list *head;
    struct mk_list *list;
    struct mc_block_info *bi;

    if (!env_path) {
        ret = mc_env_default(tmp, sizeof(tmp) - 1);
        if (ret != 0) {
            return -1;
        }
    }
    else {
        strcpy(tmp, env_path);
    }

    list = mc_block_list_create(tmp, &count);
    if (!list) {
        mc_error("[list blocks] could not read environment info: %s", tmp);
        return -1;
    }

    mk_list_foreach(head, list) {
        bi = mk_list_entry(head, struct mc_block_info, _head);

        /* Calculate Hash */
        mc_block_hash(&bi->block, (unsigned char *) &hash);

        /* Hash to Hex */
        mc_utils_hash_to_string(hash, (char *) &str_hash);
        mc_info("%s[blk %08i]%s %s %s",
                ANSI_BOLD, bi->block.block_id, ANSI_RESET, str_hash, bi->path);
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

    block = mc_block_create(tmp, NULL);
    if (!block) {
        return -1;
    }
    mc_block_save(block, tmp);

    free(block);
    return 0;
}

/* Given a Hash, retrieve information of the proper block associated */
static int cmd_info(char *str_hash)
{
    int i;
    int c;
    int ret;
    unsigned char hash[32];
    unsigned char out[32];
    char out_hash[65];
    char tmp[PATH_MAX];
    struct mk_list *list;
    struct mk_list *head;
    struct mc_block_info *b;

    /* Get environment absolute path */
    ret = mc_env_default(tmp, sizeof(tmp) - 1);
    if (ret != 0) {
        return -1;
    }

    list = mc_block_list_create(tmp, &c);
    if (!list) {
        mc_error("[hash info] error reading blocks information");
        return -1;
    }

    /* Query the last block */
    if (str_hash == NULL) {
        b = mk_list_entry_last(list, struct mc_block_info, _head);
        mc_block_print_info(b);
        mc_block_list_destroy(list);
        return 0;
    }

    /*
     * Convert string-hash to binary:
     *
     * FIXME: this routine must reverse str_hash to binary hash, for now
     * I am using the str_hash comparisson which is more expensive.
     */

    /* Iterate each block search a matching block-header hash */
    mk_list_foreach(head, list) {
        b = mk_list_entry(head, struct mc_block_info, _head);

        /* Get the block hash and compare it */
        mc_block_hash(&b->block, (unsigned char *) &out);

        mc_utils_hash_to_string((unsigned char *) &out, (char *) &out_hash);
        /* FIXME: str_hash->hash
        if (memcmp(out, hash, 32) == 0) {
            break;
        }
        */
        if (strcasecmp(str_hash, out_hash) == 0) {
            break;
        }
        b = NULL;
    }

    if (!b) {
        mc_block_list_destroy(list);
        mc_error("[hash info] no data found associated with %s", str_hash);
        return -1;
    }

    mc_block_print_info(b);
    mc_block_list_destroy(list);

    return 0;
}

int main(int argc, char **argv)
{
    int ret;
    int opt;
    int optid = 1;
    char *env_path = NULL;

    static const struct option long_opts[] = {
        {"environment", required_argument, NULL, 'e'},
        {"init"       , optional_argument, NULL, 'I'},
        {"info"       , optional_argument, NULL, 'i'},
        {"block"      , no_argument      , NULL, 'b'},
        {"list"       , no_argument      , NULL, 'l'},
        {"help"       , no_argument      , NULL, 'h'},
    };

    while ((opt = getopt_long(argc, argv, "e:I::i::blh", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'e':
            env_path = strdup(optarg);
            break;
        case 'I':
            /* Special handle for optional argument */
            if (!optarg && argv[optind] != NULL &&  \
                argv[optind][0] != '-') {
                optarg = argv[optind++];
            }
            if (env_path) {
                ret = mc_env_create(env_path);
            }
            else {
                ret = mc_env_create(optarg);
            }
            return ret;
        case 'i':
            /* Special handle for optional argument */
            if (!optarg && argv[optind] != NULL &&  \
                argv[optind][0] != '-') {
                optarg = argv[optind++];
            }
            ret = cmd_info(optarg);
            return ret;
        case 'b':
            ret = cmd_block_create();
            return ret;
        case 'l':
            ret = cmd_list_blocks(env_path);
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
