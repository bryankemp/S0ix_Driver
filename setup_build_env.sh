#!/bin/bash

#
# Build Environment Setup Script for S0ix Driver
#
# Copyright (c) 2024, Bryan Kemp
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

set -e  # Exit on any error

# Print functions
print_info() {
    echo -e "\033[0;34m[INFO]\033[0m $1"
}

print_success() {
    echo -e "\033[0;32m[SUCCESS]\033[0m $1"
}

print_warning() {
    echo -e "\033[1;33m[WARNING]\033[0m $1"
}

print_error() {
    echo -e "\033[0;31m[ERROR]\033[0m $1"
}

# Check if running as root
check_root() {
    if [[ $EUID -eq 0 ]]; then
        print_warning "This script should not be run as root for security reasons."
    fi
}

# Detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        VERSION=$VERSION_ID
    else
        DISTRO="unknown"
    fi
    
    print_info "Detected distribution: $DISTRO"
}

# Get current kernel version
get_kernel_version() {
    KERNEL_VERSION=$(uname -r)
    print_info "Current kernel version: $KERNEL_VERSION"
}

# Install required packages
install_dependencies() {
    print_info "Installing build dependencies..."
    
    if [[ "$DISTRO" == "ubuntu" || "$DISTRO" == "debian" ]]; then
        sudo apt update && sudo apt install -y linux-headers-$KERNEL_VERSION cmake build-essential
    fi
}

# Verify kernel headers installation
verify_kernel_headers() {
    if [ ! -f "/lib/modules/$KERNEL_VERSION/build/Makefile" ]; then
        print_error "Kernel headers not found!"
        exit 1
    fi
    print_success "Kernel headers verified."
}

# Setup build directory
setup_build_directory() {
    print_info "Setting up the build directory..."
    mkdir -p build
    cd build
    cmake ..
    cd ..
    print_success "Build directory setup complete."
}

# Main execution
main() {
    print_info "Configuring build environment for S0ix Driver"
    check_root
    detect_distro
    get_kernel_version
    install_dependencies
    verify_kernel_headers
    setup_build_directory
    print_success "Build environment configuration complete."
}

main "$@"

