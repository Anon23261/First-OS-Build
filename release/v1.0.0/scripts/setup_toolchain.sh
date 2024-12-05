#!/bin/bash

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${GREEN}Setting up cross-compiler toolchain...${NC}"

# Install required packages
echo "Installing required packages..."
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    nasm \
    qemu-system-x86 \
    xorriso \
    grub-pc-bin \
    grub-common

# Variables
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# Create directories
mkdir -p $HOME/src
cd $HOME/src

# Download and build binutils
if [ ! -f "binutils-2.37.tar.gz" ]; then
    wget https://ftp.gnu.org/gnu/binutils/binutils-2.37.tar.gz
    tar xf binutils-2.37.tar.gz
fi

# Build binutils
mkdir -p build-binutils
cd build-binutils
../binutils-2.37/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install

cd $HOME/src

# Download and build GCC
if [ ! -f "gcc-11.2.0.tar.gz" ]; then
    wget https://ftp.gnu.org/gnu/gcc/gcc-11.2.0/gcc-11.2.0.tar.gz
    tar xf gcc-11.2.0.tar.gz
fi

# Build GCC
mkdir -p build-gcc
cd build-gcc
../gcc-11.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc

echo -e "${GREEN}Toolchain setup complete!${NC}"
echo "Add the following to your ~/.bashrc:"
echo 'export PATH="$HOME/opt/cross/bin:$PATH"'
