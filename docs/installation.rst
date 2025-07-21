Installation
============

This guide provides step-by-step instructions for building and installing the S0ix Driver kernel module.

Prerequisites
-------------

Before building the S0ix Driver, ensure your system has the following requirements:

System Requirements
~~~~~~~~~~~~~~~~~~~

- Linux operating system (Ubuntu, Debian, RHEL, CentOS, Fedora, etc.)
- Kernel version 4.0 or later (tested on recent distributions)
- Root privileges for module loading/unloading
- Internet connection (for dependency installation)

Build Dependencies
~~~~~~~~~~~~~~~~~~

The following packages are required for building the kernel module:

- **Linux kernel headers** - Matching your current kernel version
- **CMake 3.16 or later** - Build system
- **GCC compiler** - For compiling C code
- **Make** - Build automation tool

Quick Installation (Recommended)
---------------------------------

The easiest way to get started is using the automated setup script:

.. code-block:: bash

   # Clone or navigate to the project directory
   cd s0ix_driver

   # Run the automated setup script
   ./setup_build_env.sh

   # Build and load the module  
   make s0ix_driver_module
   make s0ix_driver_load

   # View statistics
   make s0ix_driver_stats

The setup script will automatically:

- Detect your Linux distribution
- Install required dependencies
- Verify kernel headers are available
- Configure the build environment with CMake
- Provide status messages throughout the process

Manual Installation
-------------------

If you prefer to install dependencies manually or the automated script doesn't support your distribution:

Ubuntu/Debian Systems
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   sudo apt update
   sudo apt install linux-headers-$(uname -r) cmake build-essential

RHEL/CentOS/Fedora Systems
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # For newer systems (dnf)
   sudo dnf install kernel-devel kernel-headers cmake gcc make

   # For older systems (yum)
   sudo yum install kernel-devel kernel-headers cmake gcc make

Other Distributions
~~~~~~~~~~~~~~~~~~~

For other Linux distributions, install the equivalent packages:

- Kernel headers/development files for your kernel version
- CMake build system
- GCC compiler and build tools

Building the Module
-------------------

The S0ix Driver supports multiple build methods:

Method 1: Top-level Makefile (Recommended)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Build the kernel module
   make s0ix_driver_module

   # Or build everything
   make all

Method 2: CMake Direct Build
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Create and enter build directory
   mkdir build
   cd build

   # Configure the build
   cmake ..

   # Build the kernel module
   make s0ix_driver_module

Build Verification
------------------

After a successful build, you should see output similar to:

.. code-block:: text

   S0ix Driver Kernel Module Build Configuration
   ============================================

   Available targets:
     s0ix_driver_module  - Build the kernel module
     s0ix_driver_clean   - Clean build files
     s0ix_driver_install - Install the module
     s0ix_driver_load    - Load the module
     s0ix_driver_unload  - Unload the module
     s0ix_driver_stats   - View module statistics

The compiled kernel module will be located at:

- ``build/s0ix_driver.ko`` - The compiled kernel module
- ``build/manage_module.sh`` - Generated management script

Loading the Module
------------------

Once built, you can load the module using several methods:

Using Make Targets
~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Load the module
   make s0ix_driver_load

   # Check if loaded successfully
   lsmod | grep s0ix_driver

Using the Management Script
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Load the module
   ./build/manage_module.sh load

   # Check module status
   ./build/manage_module.sh status

Manual Loading
~~~~~~~~~~~~~~

.. code-block:: bash

   # Load manually
   sudo insmod build/s0ix_driver.ko

   # Verify loading
   dmesg | tail

Installation Verification
-------------------------

To verify the module is working correctly:

1. **Check module is loaded:**

   .. code-block:: bash

      lsmod | grep s0ix_driver

2. **Verify proc entry exists:**

   .. code-block:: bash

      ls -la /proc/s0ix_stats

3. **Check initial statistics:**

   .. code-block:: bash

      cat /proc/s0ix_stats

4. **View kernel messages:**

   .. code-block:: bash

      dmesg | grep s0ix_driver

Expected output should show successful initialization messages.

Troubleshooting Installation Issues
-----------------------------------

Module Won't Load
~~~~~~~~~~~~~~~~~

**Error: Invalid module format**

- Ensure kernel headers match your running kernel version
- Rebuild the module after installing correct headers

**Error: Operation not permitted**

- Ensure you're running with sudo privileges
- Check if Secure Boot is enabled (may prevent unsigned modules)

**Error: No such device**

- This is expected - the module creates a virtual platform device

Build Fails
~~~~~~~~~~~~

**Error: Kernel build directory not found**

.. code-block:: bash

   # Install kernel headers
   sudo apt install linux-headers-$(uname -r)  # Ubuntu/Debian
   sudo dnf install kernel-devel kernel-headers  # RHEL/Fedora

**Error: CMake version too old**

- Install CMake 3.16 or later
- On older systems, you may need to build CMake from source

**Error: No rule to make target**

- Ensure you're in the correct directory
- Try cleaning and rebuilding:

  .. code-block:: bash

     make clean
     make s0ix_driver_module

Uninstalling
------------

To remove the S0ix Driver:

.. code-block:: bash

   # Unload the module
   make s0ix_driver_unload
   # or
   sudo rmmod s0ix_driver

   # Clean build files
   make clean

   # Remove all build artifacts
   make distclean

This will remove all compiled files and return the project to its original state.
