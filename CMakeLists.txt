#
# S0ix Power Mode Transition Driver - CMake Build Configuration
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

cmake_minimum_required(VERSION 3.16)
project(s0ix_driver)

# Set the kernel module name
set(KMODULE_NAME "s0ix_driver")

# Find the kernel build directory
find_path(KERNEL_BUILD_DIR
    NAMES Makefile
    PATHS
        /lib/modules/${CMAKE_SYSTEM_VERSION}/build
        /usr/src/linux-headers-${CMAKE_SYSTEM_VERSION}
        /usr/src/kernels/${CMAKE_SYSTEM_VERSION}
    NO_DEFAULT_PATH
)

# If not found, try to get kernel version dynamically
if(NOT KERNEL_BUILD_DIR)
    execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    find_path(KERNEL_BUILD_DIR
        NAMES Makefile
        PATHS
            /lib/modules/${KERNEL_VERSION}/build
            /usr/src/linux-headers-${KERNEL_VERSION}
            /usr/src/kernels/${KERNEL_VERSION}
        NO_DEFAULT_PATH
    )
endif()

if(NOT KERNEL_BUILD_DIR)
    message(FATAL_ERROR "Kernel build directory not found. Please install kernel headers.")
else()
    message(STATUS "Found kernel build directory: ${KERNEL_BUILD_DIR}")
endif()

# Set the source files
set(KMODULE_SOURCES
    s0ix_driver.c
)

# Create the kernel Makefile
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild.in
    ${CMAKE_CURRENT_BINARY_DIR}/Kbuild
    @ONLY
)

# Copy source files to build directory first
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${KMODULE_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/${KMODULE_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${KMODULE_SOURCES}
    COMMENT "Copying source files to build directory"
)

# Custom target to build the kernel module
add_custom_target(${KMODULE_NAME}_module ALL
    COMMAND make -C ${KERNEL_BUILD_DIR} M=${CMAKE_CURRENT_BINARY_DIR} modules
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${KMODULE_SOURCES}
    COMMENT "Building kernel module ${KMODULE_NAME}"
    VERBATIM
)

# Custom target to clean the kernel module
add_custom_target(${KMODULE_NAME}_clean
    COMMAND make -C ${KERNEL_BUILD_DIR} M=${CMAKE_CURRENT_BINARY_DIR} clean
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Cleaning kernel module build files"
)

# Custom target to install the kernel module
add_custom_target(${KMODULE_NAME}_install
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${KERNEL_BUILD_DIR} M=${CMAKE_CURRENT_BINARY_DIR} modules_install
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${KMODULE_NAME}_module
    COMMENT "Installing kernel module ${KMODULE_NAME}"
)

# Add a custom target for loading the module
add_custom_target(${KMODULE_NAME}_load
    COMMAND sudo insmod ${CMAKE_CURRENT_BINARY_DIR}/${KMODULE_NAME}.ko
    DEPENDS ${KMODULE_NAME}_module
    COMMENT "Loading kernel module ${KMODULE_NAME}"
)

# Add a custom target for unloading the module
add_custom_target(${KMODULE_NAME}_unload
    COMMAND sudo rmmod ${KMODULE_NAME} || true
    COMMENT "Unloading kernel module ${KMODULE_NAME}"
)

# Add a custom target to view module statistics
add_custom_target(${KMODULE_NAME}_stats
    COMMAND cat /proc/s0ix_stats || echo "Module not loaded or proc entry not available"
    COMMENT "Displaying S0ix statistics"
)

# Create a helper script for easy module management
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/manage_module.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/manage_module.sh
    @ONLY
)

# Make the script executable
file(CHMOD ${CMAKE_CURRENT_BINARY_DIR}/manage_module.sh
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
)

# Print usage information
message(STATUS "")
message(STATUS "S0ix Driver Kernel Module Build Configuration")
message(STATUS "============================================")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  ${KMODULE_NAME}_module  - Build the kernel module")
message(STATUS "  ${KMODULE_NAME}_clean   - Clean build files")
message(STATUS "  ${KMODULE_NAME}_install - Install the module")
message(STATUS "  ${KMODULE_NAME}_load    - Load the module")
message(STATUS "  ${KMODULE_NAME}_unload  - Unload the module")
message(STATUS "  ${KMODULE_NAME}_stats   - View module statistics")
message(STATUS "")
message(STATUS "Top-level Makefile targets:")
message(STATUS "  clean     - Clean current build directory")
message(STATUS "  distclean - Remove all build directories")
message(STATUS "")
message(STATUS "Usage examples:")
message(STATUS "  make ${KMODULE_NAME}_module")
message(STATUS "  make ${KMODULE_NAME}_load")
message(STATUS "  make ${KMODULE_NAME}_stats")
message(STATUS "  make ${KMODULE_NAME}_unload")
message(STATUS "  make distclean    # Remove all build directories")
message(STATUS "")
message(STATUS "Or use the management script: ./manage_module.sh [load|unload|stats|reload]")
message(STATUS "For complete cleanup: make distclean")
message(STATUS "")
