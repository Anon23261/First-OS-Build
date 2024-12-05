cmake_minimum_required(VERSION 3.10)
project(CustomOS VERSION 1.0.0 LANGUAGES CXX ASM)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER nasm)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target i686-pc-none-elf -ffreestanding -fno-rtti -fno-exceptions -nostdlib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_ASM_FLAGS "-f elf32")

# Configure NASM
enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -f ${CMAKE_ASM_NASM_OBJECT_FORMAT}")

# Set linker
set(CMAKE_LINKER "ld.lld")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET>")

# Find all source files
file(GLOB_RECURSE CPP_SOURCES "kernel/*.cpp")
file(GLOB_RECURSE ASM_SOURCES "kernel/*.asm")

# Create kernel executable
add_executable(kernel.bin 
    ${CPP_SOURCES}
    ${ASM_SOURCES}
)

# Set linker script
set_target_properties(kernel.bin PROPERTIES
    LINK_FLAGS "-T ${CMAKE_SOURCE_DIR}/linker.ld -nostdlib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/iso/boot"
)

# Custom target for creating ISO
add_custom_target(iso
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/iso/boot/grub"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/grub.cfg" "${CMAKE_BINARY_DIR}/iso/boot/grub/"
    COMMAND grub-mkrescue -o "${CMAKE_BINARY_DIR}/os.iso" "${CMAKE_BINARY_DIR}/iso"
    DEPENDS kernel.bin
    COMMENT "Creating bootable ISO image..."
)

# Custom target for running in QEMU
add_custom_target(run
    COMMAND qemu-system-i386 -cdrom "${CMAKE_BINARY_DIR}/os.iso"
    DEPENDS iso
    COMMENT "Running OS in QEMU..."
)

# Custom target for debugging in QEMU
add_custom_target(debug
    COMMAND qemu-system-i386 -cdrom "${CMAKE_BINARY_DIR}/os.iso" -s -S
    DEPENDS iso
    COMMENT "Starting OS in QEMU debug mode..."
)

# Install targets
install(TARGETS kernel.bin
    RUNTIME DESTINATION boot
)

install(FILES "${CMAKE_BINARY_DIR}/os.iso"
    DESTINATION .
    OPTIONAL
)
