set(src
  endian.c
  sha256.c
  )

add_library(mc-sha256-static STATIC ${src})
