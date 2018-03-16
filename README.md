# MonkChain

A simple (and incomplete) Blockchain implementation for learning purposes.

DISCLAIMER: this code it's a basic library to create and manage blocks: no proof-of-work, no networking and no mining _yet_.

## Getting Started

Build the library and tool:

```
$ cd build/
$ cmake ../
$ make
```

Initialize the environment:

```
$ tools/mc -I
[2018/03/15 21:21:28] [ info] [create env] path /home/edsiper/.monkchain OK
[2018/03/15 21:21:28] [ info] [block] Generate block #0 hash=b4382f161a3458acf0c8184feecd32ccc466c58651ae893a03198fbcd19f6ffd
```

List Blocks

```
$ tools/mc -l
[2018/03/15 21:21:50] [ info] [blk 00000000] b4382f161a3458acf0c8184feecd32ccc466c58651ae893a03198fbcd19f6ffd /home/edsiper/.monkchain/blk00000000.dat
```

Add new Blocks

```
$ tools/mc -b
[2018/03/15 21:22:05] [ info] [block] Generate block #1 hash=4126299a02c18466208794d14b93d2c4988255617f00bc7e8c765bf5841d1776
```

Get Block Information

```
$ tools/mc -i 4126299a02c18466208794d14b93d2c4988255617f00bc7e8c765bf5841d1776
[2018/03/15 21:23:15] [ info] [info]
Block Details
 [+] hash     =>  4126299a02c18466208794d14b93d2c4988255617f00bc7e8c765bf5841d1776
 [+] parent   =>  b4382f161a3458acf0c8184feecd32ccc466c58651ae893a03198fbcd19f6ffd
 [+] size     =>  76
 [+] created  =>  2018-03-16 03:22:05

File System
 [+] path     =>  /home/edsiper/.monkchain/blk00000001.dat
 [+] size     =>  84 bytes (84b)
```



## Author

Eduardo Silva <eduardo@monkey.io>


