Troubleshooting
===============

This guide helps diagnose and resolve common issues with the S0ix Driver.

Common Issues
-------------

Module Loading Problems
~~~~~~~~~~~~~~~~~~~~~~~

Module Won't Load - Invalid Format
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

   insmod: ERROR: could not insert module build/s0ix_driver.ko: Invalid module format

**Causes:**
- Kernel headers version mismatch
- Module built for different kernel version
- Architecture mismatch (32-bit vs 64-bit)

**Solutions:**

1. **Check kernel version match:**

   .. code-block:: bash

      # Check running kernel
      uname -r

      # Check what the module was built for
      modinfo build/s0ix_driver.ko | grep vermagic

2. **Reinstall correct kernel headers:**

   .. code-block:: bash

      # Ubuntu/Debian
      sudo apt install linux-headers-$(uname -r)

      # RHEL/Fedora  
      sudo dnf install kernel-devel-$(uname -r)

3. **Clean rebuild:**

   .. code-block:: bash

      make distclean
      ./setup_build_env.sh
      make s0ix_driver_module

Module Won't Load - Operation Not Permitted
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

   insmod: ERROR: could not insert module build/s0ix_driver.ko: Operation not permitted

**Causes:**
- Insufficient privileges
- Secure Boot enabled
- Module signing required
- SELinux/AppArmor restrictions

**Solutions:**

1. **Check privileges:**

   .. code-block:: bash

      # Ensure running as root/sudo
      sudo insmod build/s0ix_driver.ko

2. **Check Secure Boot status:**

   .. code-block:: bash

      # Check if Secure Boot is enabled
      mokutil --sb-state

   If Secure Boot is enabled, you may need to:
   - Disable Secure Boot in BIOS/UEFI
   - Sign the module (advanced)
   - Use a signed kernel that allows unsigned modules

3. **Check SELinux:**

   .. code-block:: bash

      # Check SELinux status
      getenforce

      # Temporarily disable (for testing only)
      sudo setenforce 0

Module Won't Load - Required Key Not Available
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

   insmod: ERROR: could not insert module: Required key not available

**Cause:**
Module signature verification is enabled but module is unsigned.

**Solutions:**

1. **Disable module signature verification (temporary, for development):**

   .. code-block:: bash

      # Add to kernel boot parameters
      module.sig_enforce=0

2. **Sign the module (advanced):**
   This requires kernel signing keys and is beyond basic troubleshooting scope.

Build Issues
~~~~~~~~~~~~

Kernel Build Directory Not Found
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: text

   CMake Error: Kernel build directory not found. Please install kernel headers.

**Solutions:**

1. **Install kernel headers:**

   .. code-block:: bash

      # Ubuntu/Debian
      sudo apt install linux-headers-$(uname -r)

      # RHEL/Fedora
      sudo dnf install kernel-devel kernel-headers

2. **Verify installation:**

   .. code-block:: bash

      ls -la /lib/modules/$(uname -r)/build

3. **Manual path specification (if needed):**

   .. code-block:: bash

      # Set KERNEL_SRC environment variable
      export KERNEL_SRC=/usr/src/linux-headers-$(uname -r)

CMake Version Too Old
^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: text

   CMake Error: CMake 3.16 or higher is required.

**Solutions:**

1. **Ubuntu/Debian - install from repositories:**

   .. code-block:: bash

      sudo apt update
      sudo apt install cmake

2. **Install from CMake website (if repository version is too old):**

   .. code-block:: bash

      # Download and install latest CMake
      wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
      echo 'deb https://apt.kitware.com/ubuntu/ focal main' | sudo tee /etc/apt/sources.list.d/kitware.list
      sudo apt update
      sudo apt install cmake

Runtime Issues
~~~~~~~~~~~~~~

Module Loads but No Statistics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
- Module loads successfully (``lsmod | grep s0ix_driver`` shows module)
- ``/proc/s0ix_stats`` shows zero counts
- No statistics changes after suspend/resume

**Diagnosis:**

1. **Check if proc entry exists:**

   .. code-block:: bash

      ls -la /proc/s0ix_stats

2. **Check sysfs entries:**

   .. code-block:: bash

      ls -la /sys/devices/platform/s0ix_device/

3. **Check kernel messages:**

   .. code-block:: bash

      dmesg | grep s0ix_driver

**Solutions:**

1. **Verify power management support:**

   .. code-block:: bash

      # Check if system supports suspend
      cat /sys/power/state

      # Check current suspend mode
      cat /sys/power/mem_sleep

2. **Test suspend functionality:**

   .. code-block:: bash

      # Trigger suspend
      sudo systemctl suspend

3. **Check for platform device issues:**

   .. code-block:: bash

      # Look for platform device registration errors
      dmesg | grep -i "platform\|s0ix"

Statistics Not Updating
^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
- Proc entry exists and shows initial statistics
- Statistics don't change after suspend/resume cycles
- Suspend/resume appears to work normally

**Diagnosis:**

1. **Check if suspend actually triggers platform callbacks:**

   .. code-block:: bash

      # Monitor kernel messages during suspend
      sudo dmesg -w | grep s0ix &
      sudo systemctl suspend
      # Check if suspend/resume messages appear

2. **Verify module state:**

   .. code-block:: bash

      # Check module is still loaded after resume
      lsmod | grep s0ix_driver

**Solutions:**

1. **Try different suspend methods:**

   .. code-block:: bash

      # Method 1: systemctl
      sudo systemctl suspend

      # Method 2: pm-suspend  
      sudo pm-suspend

      # Method 3: direct sysfs
      echo mem | sudo tee /sys/power/state

2. **Check platform device binding:**

   .. code-block:: bash

      # Check if device is properly bound to driver
      ls -la /sys/bus/platform/drivers/s0ix_device/

Performance Issues
~~~~~~~~~~~~~~~~~~

System Slow During Resume
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
- System takes much longer than normal to resume
- 11+ second delay after resume

**Cause:**
The module includes a deliberate 11-second delay in the resume path for demonstration purposes.

**Solution:**
This is expected behavior. To modify or remove the delay, edit the source code:

.. code-block:: c

   // In s0ix_device_resume() function
   // Comment out or modify this line:
   msleep(11000);  // Remove or reduce this delay

System Crashes During Suspend/Resume
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
- System crashes or hangs during suspend/resume with module loaded
- Kernel panic messages mentioning s0ix_driver

**Immediate action:**

1. **Unload the module:**

   .. code-block:: bash

      sudo rmmod s0ix_driver

2. **Check kernel logs:**

   .. code-block:: bash

      dmesg | grep -A 10 -B 10 s0ix_driver

**Solutions:**
- This indicates a serious bug in the module
- Check for memory corruption or race conditions
- Report the issue with full kernel logs and system information

Debugging Techniques
--------------------

Enable Kernel Debug Messages
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Enable dynamic debug for the module
   echo 'module s0ix_driver +p' | sudo tee /sys/kernel/debug/dynamic_debug/control

   # Or enable all kernel debug messages (very verbose)
   echo 8 | sudo tee /proc/sys/kernel/printk

Monitor Kernel Messages
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Follow kernel messages in real-time
   sudo dmesg -w

   # Filter for specific module messages
   sudo dmesg -w | grep s0ix

   # View recent messages
   dmesg | tail -50

Check Module Information
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # View module details
   modinfo build/s0ix_driver.ko

   # Check module dependencies
   modprobe --show-depends s0ix_driver

   # List all loaded modules
   lsmod

Verify System Power Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Check available power states
   cat /sys/power/state

   # Check suspend-to-idle support
   cat /sys/power/mem_sleep

   # Check wakeup sources
   cat /sys/kernel/debug/wakeup_sources

Advanced Debugging
~~~~~~~~~~~~~~~~~~~

Using ftrace
^^^^^^^^^^^^

.. code-block:: bash

   # Enable function tracing
   echo function > /sys/kernel/debug/tracing/current_tracer
   echo s0ix_device_suspend > /sys/kernel/debug/tracing/set_ftrace_filter
   echo 1 > /sys/kernel/debug/tracing/tracing_on

   # Trigger suspend and check trace
   sudo systemctl suspend
   cat /sys/kernel/debug/tracing/trace

Memory Debugging
^^^^^^^^^^^^^^^^

.. code-block:: bash

   # Enable KASAN if available (requires kernel recompilation)
   # Check for memory leaks
   cat /sys/kernel/debug/kmemleak

Getting Help
------------

Before Seeking Support
~~~~~~~~~~~~~~~~~~~~~~

Gather the following information:

1. **System information:**

   .. code-block:: bash

      uname -a
      cat /etc/os-release
      lscpu | head -20

2. **Kernel configuration:**

   .. code-block:: bash

      cat /boot/config-$(uname -r) | grep -i power
      cat /boot/config-$(uname -r) | grep -i suspend

3. **Build information:**

   .. code-block:: bash

      modinfo build/s0ix_driver.ko
      cmake --version
      gcc --version

4. **Runtime information:**

   .. code-block:: bash

      lsmod | grep s0ix
      dmesg | grep s0ix_driver
      cat /proc/s0ix_stats

5. **Power management status:**

   .. code-block:: bash

      cat /sys/power/state
      cat /sys/power/mem_sleep

Community Resources
~~~~~~~~~~~~~~~~~~~

- Check the project's issue tracker
- Consult Linux kernel documentation
- Review power management subsystem documentation
- Search Linux kernel mailing list archives

When reporting issues, include all the gathered information above plus:

- Steps to reproduce the problem
- Expected vs actual behavior
- Any error messages or kernel panics
- Hardware information (especially power management features)
