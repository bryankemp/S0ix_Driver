#!/bin/bash

#
# Documentation Build Script for S0ix Driver
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

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the docs directory
if [ ! -f "conf.py" ]; then
    print_error "This script must be run from the docs/ directory"
    exit 1
fi

print_status "Building S0ix Driver documentation..."

# Check if virtual environment exists, create if not
if [ ! -d "venv" ]; then
    print_status "Creating Python virtual environment..."
    python3 -m venv venv
fi

# Activate virtual environment
print_status "Activating virtual environment..."
source venv/bin/activate

# Install/upgrade requirements
print_status "Installing documentation dependencies..."
pip install -r requirements.txt

# Clean previous build
print_status "Cleaning previous build..."
make clean

# Build HTML documentation
print_status "Building HTML documentation..."
make html

if [ $? -eq 0 ]; then
    print_success "Documentation built successfully!"
    print_status "Documentation available at: _build/html/index.html"
    print_status "To view: open _build/html/index.html"
else
    print_error "Documentation build failed!"
    exit 1
fi

# Optionally build PDF if requested
if [ "$1" = "pdf" ]; then
    print_status "Building PDF documentation..."
    make latexpdf
    
    if [ $? -eq 0 ]; then
        print_success "PDF documentation built successfully!"
        print_status "PDF available at: _build/latex/S0ixDriver.pdf"
    else
        print_warning "PDF build failed (this is normal if LaTeX is not installed)"
    fi
fi

print_success "Documentation build complete!"
