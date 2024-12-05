FROM --platform=linux/amd64 ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Update and install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    lld \
    nasm \
    xorriso \
    grub-pc-bin \
    grub-common \
    cmake \
    make \
    qemu-system-x86 \
    gdb \
    git \
    curl \
    python3 \
    python3-pip \
    dos2unix \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -s /bin/bash developer \
    && echo "developer ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Set up development environment
USER developer
WORKDIR /workspace

# Install additional tools
RUN pip3 install --user cmake-format cpp-coveralls

# Add aliases for common commands
RUN echo 'alias build="./scripts/build.sh"' >> ~/.bashrc \
    && echo 'alias run="./scripts/run.sh"' >> ~/.bashrc \
    && echo 'alias debug="./scripts/run.sh debug"' >> ~/.bashrc \
    && echo 'alias check="./scripts/check-system.sh"' >> ~/.bashrc

CMD ["/bin/bash"]
