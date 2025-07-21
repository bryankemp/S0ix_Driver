Development
===========

This guide provides information for developers who want to contribute to or modify the S0ix Driver.

Development Environment Setup
-----------------------------

Prerequisites
~~~~~~~~~~~~~

- Linux development environment
- Kernel headers for target kernel version(s)
- Git for version control
- Text editor or IDE with C syntax highlighting
- Understanding of Linux kernel module development

Setting Up for Development
~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **Clone the repository:**

   .. code-block:: bash

      git clone <repository-url>
      cd s0ix_driver

2. **Set up the build environment:**

   .. code-block:: bash

      ./setup_build_env.sh

3. **Build and test:**

   .. code-block:: bash

      make s0ix_driver_module
      make s0ix_driver_load
      make s0ix_driver_stats

Code Structure
--------------

Project Layout
~~~~~~~~~~~~~~

.. code-block:: text

   s0ix_driver/
   ├── build/                  # Build directory (generated)
   ├── docs/                   # Documentation source
   ├── CMakeLists.txt          # Main CMake configuration  
   ├── Kbuild.in              # Kernel build template
   ├── LICENSE                 # BSD 3-Clause License
   ├── Makefile               # Top-level convenience Makefile
   ├── manage_module.sh.in     # Module management script template
   ├── README.md              # Project overview
   ├── s0ix_driver.c          # Main kernel module source
   └── setup_build_env.sh     # Build environment setup script

Source Code Organization
~~~~~~~~~~~~~~~~~~~~~~~~

The main module source (``s0ix_driver.c``) is organized into logical sections:

1. **Header Includes and Definitions**
2. **Data Structures** - Statistics tracking structure
3. **Sysfs Interface Functions** - Individual attribute handlers
4. **Power Management Callbacks** - Suspend/resume handlers
5. **Proc Interface Functions** - Statistics display functions  
6. **Module Lifecycle Functions** - Init/exit handlers
7. **Module Metadata** - Author, description, license information

Key Components
~~~~~~~~~~~~~~

Statistics Structure
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   struct s0ix_stats {
       unsigned long suspend_count;        
       unsigned long resume_count;         
       u64 total_suspend_time_ms;          
       ktime_t last_suspend_time;          
   };

This structure tracks all power management statistics with thread-safe access protected by a mutex.

Platform Driver Integration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The module registers as a platform driver to receive power management callbacks:

.. code-block:: c

   static struct platform_driver s0ix_platform_driver = {
       .driver = {
           .name = S0IX_DEVICE_NAME,
           .owner = THIS_MODULE,
           .pm = &s0ix_pm_ops,
       },
   };

Multiple Interface Support
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **Proc filesystem** - Human-readable statistics via ``/proc/s0ix_stats``
- **Sysfs attributes** - Individual values for programmatic access
- **Kernel logging** - Real-time event messages via ``printk``

Development Workflow
--------------------

Making Changes
~~~~~~~~~~~~~~

1. **Create a development branch:**

   .. code-block:: bash

      git checkout -b feature/your-feature-name

2. **Make your changes to the source code**

3. **Build and test:**

   .. code-block:: bash

      make clean
      make s0ix_driver_module

4. **Test the changes:**

   .. code-block:: bash

      # Unload existing module if loaded
      make s0ix_driver_unload
      
      # Load modified module
      make s0ix_driver_load
      
      # Test functionality
      make s0ix_driver_stats
      sudo systemctl suspend  # Test suspend/resume

5. **Verify no regressions:**

   .. code-block:: bash

      # Check kernel messages for errors
      dmesg | grep s0ix_driver
      
      # Verify all interfaces work
      cat /proc/s0ix_stats
      ls /sys/devices/platform/s0ix_device/

Debugging Development Changes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Enable Debug Output
^^^^^^^^^^^^^^^^^^^

Add debug messages to your code:

.. code-block:: c

   pr_debug("Debug message: %s\n", "debug info");

Enable dynamic debugging:

.. code-block:: bash

   echo 'module s0ix_driver +p' | sudo tee /sys/kernel/debug/dynamic_debug/control

Use Kernel Debugging Tools
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   # Monitor kernel messages in real-time
   sudo dmesg -w | grep s0ix

   # Use ftrace for function tracing
   echo function > /sys/kernel/debug/tracing/current_tracer

Memory Debugging
^^^^^^^^^^^^^^^^

.. code-block:: bash

   # Check for memory leaks (if KMEMLEAK enabled)
   echo scan > /sys/kernel/debug/kmemleak
   cat /sys/kernel/debug/kmemleak

Code Style and Standards
------------------------

Kernel Coding Style
~~~~~~~~~~~~~~~~~~~~

Follow the Linux kernel coding style guidelines:

- Use tabs for indentation (8-character width)
- Keep lines under 80 characters when possible
- Use kernel naming conventions (lowercase with underscores)
- Place braces on the same line for functions and control structures

Example:

.. code-block:: c

   static int s0ix_device_suspend(struct device *dev)
   {
       mutex_lock(&s0ix_device_lock);
       s_stats.suspend_count++;
       s_stats.last_suspend_time = ktime_get();
       pr_info("%s: Entering S0ix\n", S0IX_DEVICE_NAME);
       mutex_unlock(&s0ix_device_lock);
       return 0;
   }

Error Handling
~~~~~~~~~~~~~~

Always handle errors properly:

.. code-block:: c

   ret = platform_driver_register(&s0ix_platform_driver);
   if (ret) {
       pr_err("%s: Failed to register platform driver: %d\n", 
              S0IX_DEVICE_NAME, ret);
       return ret;
   }

Memory Management
~~~~~~~~~~~~~~~~~

- Minimize dynamic allocations
- Always free allocated memory in error paths
- Use appropriate cleanup in module exit function

Thread Safety
~~~~~~~~~~~~~

- Use mutexes to protect shared data structures
- Keep critical sections minimal
- Avoid sleeping while holding locks

Contributing Guidelines
-----------------------

Submitting Changes
~~~~~~~~~~~~~~~~~~

1. **Ensure your changes follow coding standards**
2. **Test thoroughly on multiple kernel versions if possible**
3. **Update documentation as needed**
4. **Create clear commit messages:**

   .. code-block:: text

      module: brief description of change
      
      Longer explanation of what this change does and why it's needed.
      Include any relevant background information or consequences.
      
      Signed-off-by: Your Name <your.email@example.com>

5. **Submit a pull request with:**
   - Clear description of changes
   - Testing performed
   - Any known limitations or issues

Testing Checklist
~~~~~~~~~~~~~~~~~~

Before submitting changes, verify:

- [ ] Module compiles without warnings
- [ ] Module loads and unloads cleanly
- [ ] All interfaces (proc/sysfs/logging) work correctly
- [ ] Suspend/resume functionality works as expected
- [ ] No kernel crashes or memory leaks
- [ ] Documentation updated if needed

Common Development Tasks
------------------------

Adding New Statistics
~~~~~~~~~~~~~~~~~~~~~

1. **Extend the statistics structure:**

   .. code-block:: c

      struct s0ix_stats {
          // existing fields...
          u64 new_statistic;
      };

2. **Add sysfs attribute:**

   .. code-block:: c

      static ssize_t new_statistic_show(struct device *dev,
                                        struct device_attribute *attr, 
                                        char *buf)
      {
          ssize_t ret;
          mutex_lock(&s0ix_device_lock);
          ret = scnprintf(buf, PAGE_SIZE, "%llu\n", s_stats.new_statistic);
          mutex_unlock(&s0ix_device_lock);
          return ret;
      }
      static DEVICE_ATTR_RO(new_statistic);

3. **Add to attribute array:**

   .. code-block:: c

      static struct attribute *s0ix_attrs[] = {
          // existing attributes...
          &dev_attr_new_statistic.attr,
          NULL
      };

4. **Update proc output format**

Modifying Power Management Behavior
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The suspend/resume callbacks can be extended to:

- Track additional timing information
- Monitor different power states
- Integrate with other kernel subsystems
- Collect hardware-specific data

Example addition:

.. code-block:: c

   static int s0ix_device_suspend(struct device *dev)
   {
       mutex_lock(&s0ix_device_lock);
       s_stats.suspend_count++;
       s_stats.last_suspend_time = ktime_get();
       
       // New functionality
       s_stats.suspend_reason = get_suspend_reason();
       
       pr_info("%s: Entering S0ix (reason: %d)\n", 
               S0IX_DEVICE_NAME, s_stats.suspend_reason);
       mutex_unlock(&s0ix_device_lock);
       return 0;
   }

Building for Different Kernels
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Test compatibility across kernel versions:

.. code-block:: bash

   # Build for specific kernel version
   export KERNEL_SRC=/usr/src/linux-headers-5.15.0-generic
   make clean
   make s0ix_driver_module

Advanced Development Topics
---------------------------

Integration with Other Subsystems
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The module can be extended to integrate with:

- ACPI subsystem for hardware power state information
- Thermal management for temperature-aware suspend decisions
- CPU frequency scaling for performance/power optimization
- Network subsystem for wake-on-LAN functionality

Performance Optimization
~~~~~~~~~~~~~~~~~~~~~~~~

- Minimize time spent in suspend/resume callbacks
- Use efficient data structures for statistics collection
- Consider using per-CPU variables for high-frequency events
- Optimize mutex usage to reduce contention

Security Considerations
~~~~~~~~~~~~~~~~~~~~~~~

- Validate all input parameters
- Implement proper permission checks for sysfs attributes
- Avoid exposing sensitive system information
- Consider rate limiting for frequent operations

Cross-Architecture Compatibility
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When developing for multiple architectures:

- Use portable data types (``u64``, ``ktime_t``)
- Avoid architecture-specific assembly code
- Test on different endianness systems
- Consider alignment requirements for data structures
