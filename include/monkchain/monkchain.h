/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  BChain
 *  ======
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

#ifndef MCHAIN_H
#define MCHAIN_H

#include <monkey/mk_core.h>
#include <monkchain/mc_macros.h>
#include <monkchain/mc_blockchain.h>
#include <monkchain/mc_log.h>
#include <monkchain/mc_utils.h>
#include <monkchain/mc_env.h>

#define MC_MAGIC_NUMBER  0xEE004B4D
#define MC_VERSION       1
#define MC_GENESIS_INIT    "Blockchain Learning..."
#define MC_GENESIS_TS      0011223455

/* File system blocks related info */
#define MCHAIN_ENV_PATH        ".monkchain"
#define MCHAIN_FILE_FMT        "blk%08i.dat"

/* Fixed values to be replaced in the future by a good algorithm */
#define MCHAIN_DIFFICULTY      2

#endif
