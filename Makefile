#
# Top-level Makefile for s0ix_driver project
# This delegates all targets to the build directory
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

BUILD_DIR = build

# Default target
.DEFAULT_GOAL := all

# Create build directory if it doesn't exist and run cmake
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake ..

# All targets delegate to the build directory
%: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) $@

# Special targets that don't require arguments
all: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) all

clean: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) clean

# Deep clean - removes all build directories
distclean:
	@echo "Removing build directories..."
	@rm -rf $(BUILD_DIR)
	@rm -rf cmake-build-debug
	@rm -rf cmake-build-release
	@echo "Build directories removed."

help: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) help

# Kernel module specific targets
s0ix_driver_module: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) s0ix_driver_module

s0ix_driver_load: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) s0ix_driver_load

s0ix_driver_unload: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) s0ix_driver_unload

s0ix_driver_clean: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) s0ix_driver_clean

s0ix_driver_install: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) s0ix_driver_install

s0ix_driver_stats: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) s0ix_driver_stats

.PHONY: all clean distclean help s0ix_driver_module s0ix_driver_load s0ix_driver_unload s0ix_driver_clean s0ix_driver_install s0ix_driver_stats

