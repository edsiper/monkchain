cmake_minimum_required(VERSION 2.8)
project(monkchain)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

include_directories(
  include/
  lib/monkey/include
  lib/
  )

# Build Options
# =============
option(MC_DEV   "Development mode"   Off)

if(MC_DEV)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Dependencies
# ============

# Monkey HTTP Server
macro(MK_SET_OPTION option value)
  set(${option} ${value} CACHE "" INTERNAL FORCE)
endmacro()
MK_SET_OPTION(MK_SYSTEM_MALLOC   On)
MK_SET_OPTION(MK_STATIC_LIB_MODE On)
add_subdirectory(lib/monkey/ EXCLUDE_FROM_ALL)

# Crypto
add_subdirectory(lib/sha256 EXCLUDE_FROM_ALL)

# MonkChain library
add_subdirectory(libmonkchain)

# Cli Tools
add_subdirectory(tools)
