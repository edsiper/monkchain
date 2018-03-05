# MonkChain

A simple and incomplete Blockchain implementation for learning purposes.

DISCLAIMER: this code it's a basic library to create and manage blocks: no proof-of-work, no networking and no mining yet.

## Getting Started

Build the library and tool:

```
$ cd build/
$ cmake ../
$ make
```

Initialize the environment:

```
$ tools/mc -i
[2018/03/05 10:33:26] [ info] [create env] path /home/edsiper/.monkchain OK
[2018/03/05 10:33:26] [ info] [block] Generate block #0 hash=cc71301d8d6fba08d5eb3933ffb191413dfbda7669691ba0d6cee757ed7bba3f
```

List Blocks

```
$ tools/mc -l
[blk 00000000] cc71301d8d6fba08d5eb3933ffb191413dfbda7669691ba0d6cee757ed7bba3f /home/edsiper/.monkchain/blk00000000.dat
```

Add new Blocks

```
$ tools/mc -b
[2018/03/05 10:33:50] [ info] [block] Generate block #1 hash=8bbd2b7496c11933719e8e3f7e9eb8f300e90e3ad5f6b4afd51213beb1d66f82
hash=8bbd2b7496c11933719e8e3f7e9eb8f300e90e3ad5f6b4afd51213beb1d66f82
```

## Author

Eduardo Silva <eduardo@monkey.io>


