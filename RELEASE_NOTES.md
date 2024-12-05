# Release Notes - v1.0.0

## Overview
First stable release of our custom operating system featuring a text editor and basic system functionality.

## Features
- Custom bootloader using GRUB
- Text editor with basic editing capabilities
- Full keyboard support with special keys
- Basic filesystem operations
- Memory management system
- Interrupt handling
- Text-mode display output

## Installation
1. Download the ISO file from the release package
2. Create a bootable USB drive or run in QEMU
3. For QEMU: `qemu-system-i386 -cdrom os.iso`

## System Requirements
- x86 compatible system
- 32MB RAM minimum
- VGA-compatible display

## Known Limitations
- Editor buffer size limited to 4KB
- Basic text editing features only
- Single file editing at a time

## Bug Fixes
- Fixed keyboard buffer overflow issues
- Improved screen refresh handling
- Better error messages for file operations
- Fixed memory management bugs

## Future Plans
- Increase editor buffer size
- Add multi-file support
- Implement more advanced text editing features
- Add support for more keyboard layouts

## Support
Report issues and suggestions through the project's issue tracker.
