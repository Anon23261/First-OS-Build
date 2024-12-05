# Changelog

## [2.0.0] - Latest Release

### Major Improvements
- Complete rewrite of the text editor with improved stability
- Enhanced memory management with proper allocation/deallocation
- Improved keyboard input handling with better special key support
- New status line showing file info and cursor position
- Better screen refresh logic with proper cursor positioning
- Added save confirmation messages
- Fixed all compilation errors and undefined references

### Technical Improvements
- Fixed interrupt handling and initialization
- Proper implementation of keyboard buffer
- Better memory management in editor operations
- Improved error handling throughout the system
- Fixed linking issues with C/C++ mixed code

## [1.1.1] - Previous Release

### Improvements
- Enhanced keyboard handling with better special key support
- Improved text editor cursor movement and screen rendering
- Better memory management and allocation
- Updated build system with improved error handling
- Added comprehensive documentation and examples

### Bug Fixes
- Fixed keyboard buffer overflow issues
- Improved error handling in file operations
- Fixed screen refresh glitches
- Better memory cleanup on file operations

## [1.0.0] - First Release

### Added
- Custom bootloader using GRUB
- Text editor with basic editing capabilities
- Keyboard input handling with special key support
- Basic filesystem operations
- Memory management system
- Interrupt handling
- Text-mode display output

### Features
- Full keyboard support with special keys (arrows, ctrl, shift)
- Text editor with cursor movement and basic editing
- File operations (create, read, write)
- Status bar with file info and editor state
- Memory management and interrupt handling

### Technical Details
- Built with Clang/LLVM toolchain
- GRUB multiboot compliant
- Supports both QEMU and real hardware
- Debug symbols included for GDB debugging
