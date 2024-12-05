# First-OS-Build

A custom operating system built from scratch with a text editor, keyboard input handling, and basic filesystem support.

## Features

- Custom bootloader using GRUB
- Text editor with basic editing capabilities
- Keyboard input handling with special key support
- Basic filesystem operations
- Memory management system
- Interrupt handling
- Text-mode display output

## Building from Source

### Prerequisites
- NASM (Netwide Assembler)
- LLVM/Clang compiler
- CMake (3.12 or higher)
- QEMU for testing
- GRUB bootloader tools
- GDB (optional, for debugging)

### Build Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/Anon23261/First-OS-Build.git
   cd First-OS-Build
   ```

2. Build the project:
   ```bash
   # Make scripts executable
   chmod +x scripts/*.sh

   # Build everything (kernel + ISO)
   ./scripts/build.sh
   ```

3. Run in QEMU:
   ```bash
   ./scripts/run.sh
   ```

### Debugging
- GDB debugging is supported
- Connect to QEMU using GDB on port 1234
- Debug symbols are included in the build

## Prerequisites

The following tools are required to build the OS:

- clang++ (for C++ compilation)
- nasm (for assembly)
- lld (for linking)
- xorriso (for ISO creation)
- GRUB (for bootloader)
- CMake (for build system)
- Make (for build automation)
- QEMU (for testing, optional)

### Installing Prerequisites

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install clang lld nasm xorriso grub-pc-bin grub-common cmake make qemu-system-x86
```

On Arch Linux:
```bash
sudo pacman -S clang lld nasm xorriso grub cmake make qemu
```

On Fedora:
```bash
sudo dnf install clang lld nasm xorriso grub2 cmake make qemu-system-x86
```

## Running the OS

### Using QEMU

To run the OS in QEMU:
```bash
./scripts/run.sh
```

Or manually:
```bash
qemu-system-i386 -cdrom build/os.iso
```

### On Real Hardware

1. Write the ISO to a USB drive (replace `/dev/sdX` with your USB device):
```bash
sudo dd if=build/os.iso of=/dev/sdX bs=4M status=progress
```

2. Boot your computer from the USB drive.

**Warning**: Be very careful with the `dd` command and make sure you specify the correct device!

## Development

### Project Structure

- `boot/`: Bootloader and assembly code
- `kernel/`: Core kernel components
  - `kernel.cpp/h`: Main kernel code
  - `keyboard.cpp/h`: Keyboard input handling
  - `editor.cpp/h`: Text editor implementation
  - `filesystem.cpp/h`: Basic filesystem operations
  - `memory.cpp/h`: Memory management
  - `interrupts.cpp/h`: Interrupt handling
- `scripts/`: Build and utility scripts
- `tests/`: Test files
- `build/`: Build output (created during build)

### Debugging

1. Start QEMU with GDB server:
```bash
./scripts/run.sh debug
```

2. In another terminal, connect GDB:
```bash
gdb
(gdb) target remote localhost:1234
```

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Known Issues

- Limited filesystem support
- No process management yet
- Basic text editor functionality

## Future Plans

- Implement process management
- Add more filesystem operations
- Enhance text editor features
- Add networking support
- Implement a basic shell

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Version
Current version: 2.0.1
