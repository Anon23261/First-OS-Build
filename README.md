# Custom Operating System

A minimal operating system built from scratch.

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

On Ubuntu/Debian, you can install these with:
```bash
sudo apt-get update
sudo apt-get install clang lld nasm xorriso grub-pc-bin grub-common cmake make qemu-system-x86
```

## Building

### Quick Start
```bash
# Build everything (kernel + ISO)
./build.sh

# Clean build directory
./build.sh clean

# Rebuild everything from scratch
./build.sh rebuild

# Only create ISO
./build.sh iso
```

### Build Options

You can customize the build process with environment variables:

- `DEBUG=1`: Enable debug output
- `JOBS=N`: Set number of parallel compilation jobs
- `CXX=compiler`: Use a different C++ compiler
- `AS=assembler`: Use a different assembler
- `LD=linker`: Use a different linker

Example:
```bash
DEBUG=1 JOBS=4 ./build.sh
```

## Testing

To test in QEMU:
```bash
qemu-system-i386 -cdrom build/minios.iso
```

## Project Structure

- `kernel/`: Kernel source files
- `boot/`: Boot-related files
- `scripts/`: Build scripts
- `docs/`: Documentation
- `build/`: Build output (created during build)

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.
