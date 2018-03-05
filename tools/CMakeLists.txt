set(src
  mc.c)

add_executable(mc ${src})
target_link_libraries(mc mc-sha256-static monkchain-static)
