# Target system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)

# Toolchain settings
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER nasm)
set(CMAKE_LINKER ld.lld)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target i686-pc-none-elf -ffreestanding -nostdlib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target i686-pc-none-elf -ffreestanding -nostdlib -fno-rtti -fno-exceptions")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -f elf32")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/linker.ld -nostdlib")

# Search settings
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
