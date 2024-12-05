# Installation Guide

## Prerequisites

- QEMU for testing (qemu-system-i386)
- Cross-compiler toolchain (i686-elf)
- NASM assembler
- Make and CMake
- Grub bootloader tools

## Quick Start

1. Extract the release package:
   ```bash
   tar xzf release-v1.0.0.tar.gz
   cd v1.0.0
   ```

2. Build the OS:
   ```bash
   ./scripts/build.sh
   ```

3. Run in QEMU:
   ```bash
   ./scripts/run.sh
   ```

## Building from Source

1. Setup the toolchain:
   ```bash
   ./scripts/setup_toolchain.sh
   source ~/.bashrc  # To update PATH
   ```

2. Build the project:
   ```bash
   make clean
   make
   ```

3. Create bootable ISO:
   ```bash
   ./scripts/build.sh
   ```

## Running on Real Hardware

1. Create a bootable USB drive:
   ```bash
   sudo dd if=build/os.iso of=/dev/sdX bs=4M status=progress
   ```
   Replace /dev/sdX with your USB device.

2. Boot from the USB drive.

## Editor Usage

- Arrow keys: Move cursor
- Ctrl+S: Save file
- Ctrl+Q: Quit editor
- Ctrl+X: Cut line
- Ctrl+V: Paste
- Backspace: Delete character

## Troubleshooting

1. If build fails:
   - Check if all tools are installed
   - Run ./scripts/check-system.sh
   - Ensure PATH includes toolchain

2. If QEMU fails:
   - Check QEMU installation
   - Try running with -debug flag

3. If keyboard doesn't work:
   - Check keyboard layout settings
   - Verify PS/2 controller initialization

## Support

Report issues at the project repository or contact the maintainers.
