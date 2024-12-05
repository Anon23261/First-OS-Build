# Connect to QEMU's GDB server
target remote localhost:1234

# Set architecture to i386
set architecture i386

# Don't stop on SIGTRAP
handle SIGTRAP nostop noprint

# Load debug symbols
symbol-file build/kernel.bin

# Set disassembly flavor to Intel
set disassembly-flavor intel

# Enable pretty printing
set print pretty on

# Set breakpoint at kernel entry
break kernel_main

# Some helpful macros
define regs
    info registers
end

define stack
    x/16xw $esp
end

define bt
    backtrace
end

# Print status
printf "GDB configuration loaded.\n"
printf "Available commands:\n"
printf "  regs  - Show registers\n"
printf "  stack - Show stack\n"
printf "  bt    - Show backtrace\n"
printf "\nType 'c' to continue execution.\n"
