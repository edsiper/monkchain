set(src
  mc_os.c
  mc_log.c
  mc_blockchain.c
  )

add_library(monkchain-static STATIC ${src})
target_link_libraries(monkchain-static mc-sha256-static monkey-core-static)
