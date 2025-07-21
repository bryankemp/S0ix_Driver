API Reference
=============

This section provides detailed technical information about the S0ix Driver's internal structures, functions, and interfaces.

Data Structures
---------------

s0ix_stats Structure
~~~~~~~~~~~~~~~~~~~~

The core statistics structure that tracks power management events:

.. code-block:: c

   struct s0ix_stats {
       unsigned long suspend_count;        // Number of suspend operations
       unsigned long resume_count;         // Number of resume operations  
       u64 total_suspend_time_ms;          // Total time spent suspended (in ms)
       ktime_t last_suspend_time;          // Timestamp when last suspend started
   };

Fields:

- ``suspend_count`` - Counter incremented each time the system prepares for suspend
- ``resume_count`` - Counter incremented each time the system resumes from suspend
- ``total_suspend_time_ms`` - Cumulative time spent in suspend state (milliseconds)
- ``last_suspend_time`` - High-resolution timestamp captured at suspend initiation

Kernel Interfaces
-----------------

Platform Driver Callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~

The module implements platform driver callbacks to receive power management notifications:

s0ix_device_suspend()
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   static int s0ix_device_suspend(struct device *dev)

Called when the system is preparing to enter suspend state.

**Parameters:**
- ``dev`` - Pointer to the device structure

**Returns:**
- ``0`` on success

**Behavior:**
- Increments ``suspend_count``
- Records current timestamp in ``last_suspend_time``
- Logs suspend initiation to kernel messages
- Uses mutex protection for thread-safe access

s0ix_device_resume()
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   static int s0ix_device_resume(struct device *dev)

Called when the system is resuming from suspend state.

**Parameters:**
- ``dev`` - Pointer to the device structure

**Returns:**
- ``0`` on success

**Behavior:**
- Increments ``resume_count``
- Calculates suspend duration and updates ``total_suspend_time_ms``
- Logs resume completion with duration to kernel messages
- Introduces 11-second delay to simulate processing (for demonstration)
- Uses mutex protection for thread-safe access

User Interface APIs
-------------------

Proc Filesystem Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~

The module provides a proc entry at ``/proc/s0ix_stats`` for human-readable statistics.

s0ix_stats_show()
^^^^^^^^^^^^^^^^^

.. code-block:: c

   static int s0ix_stats_show(struct seq_file *m, void *v)

Formats and displays statistics via the proc interface.

**Parameters:**
- ``m`` - Sequence file structure for output
- ``v`` - Unused parameter

**Returns:**
- ``0`` on success

**Output format:**

.. code-block:: text

   S0ix Power Mode Transition Driver
   =====================================

   Statistics:
     Total suspend cycles: <suspend_count>
     Total resume events: <resume_count>
     Total suspend time: <total_suspend_time_ms> ms
     Average suspend time: <calculated_average> ms

Sysfs Attribute Interface
~~~~~~~~~~~~~~~~~~~~~~~~~

Individual statistics are exposed through sysfs attributes at ``/sys/devices/platform/s0ix_device/``:

suspend_count_show()
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   static ssize_t suspend_count_show(struct device *dev,
                                    struct device_attribute *attr, char *buf)

Returns the current suspend count.

**Returns:** Number of bytes written to buffer

resume_count_show()
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   static ssize_t resume_count_show(struct device *dev,
                                   struct device_attribute *attr, char *buf)

Returns the current resume count.

**Returns:** Number of bytes written to buffer

total_suspend_time_show()
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   static ssize_t total_suspend_time_show(struct device *dev,
                                         struct device_attribute *attr, char *buf)

Returns the cumulative suspend time in milliseconds.

**Returns:** Number of bytes written to buffer

avg_suspend_time_show()
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   static ssize_t avg_suspend_time_show(struct device *dev,
                                       struct device_attribute *attr, char *buf)

Calculates and returns the average suspend time in milliseconds.

**Returns:** Number of bytes written to buffer

**Calculation:** ``total_suspend_time_ms / resume_count`` (if resume_count > 0)

Module Lifecycle Functions
--------------------------

s0ix_driver_init()
~~~~~~~~~~~~~~~~~~

.. code-block:: c

   static int __init s0ix_driver_init(void)

Module initialization function called when the module is loaded.

**Returns:**
- ``0`` on success
- Negative error code on failure

**Behavior:**
- Initializes statistics structure to zero
- Registers platform driver with the kernel
- Creates and registers platform device
- Creates sysfs attribute group
- Creates proc filesystem entry
- Sets up all necessary resources

**Error handling:** Properly cleans up partially initialized resources on failure

s0ix_driver_exit()
~~~~~~~~~~~~~~~~~~

.. code-block:: c

   static void __exit s0ix_driver_exit(void)

Module cleanup function called when the module is unloaded.

**Behavior:**
- Removes proc filesystem entry
- Removes sysfs attribute group  
- Unregisters platform device
- Unregisters platform driver
- Ensures clean resource deallocation

Thread Safety
-------------

Synchronization Mechanism
~~~~~~~~~~~~~~~~~~~~~~~~~~

The module uses a mutex for thread-safe access to shared data:

.. code-block:: c

   static DEFINE_MUTEX(s0ix_device_lock);

**Protected operations:**
- Statistics updates in suspend/resume callbacks
- Statistics reads in proc/sysfs interfaces
- All accesses to the ``s0ix_stats`` structure

**Lock ordering:** Single mutex eliminates deadlock concerns

**Performance considerations:** 
- Minimal lock hold times
- Non-blocking operations within critical sections
- Efficient mutex implementation

Constants and Definitions
-------------------------

Module Constants
~~~~~~~~~~~~~~~~

.. code-block:: c

   #define S0IX_DEVICE_NAME "s0ix_device"
   #define PROC_STATS_NAME "s0ix_stats"

**S0IX_DEVICE_NAME:** Platform device name used for registration and sysfs paths

**PROC_STATS_NAME:** Proc filesystem entry name

Module Metadata
~~~~~~~~~~~~~~~

.. code-block:: c

   MODULE_AUTHOR("Bryan Kemp");
   MODULE_DESCRIPTION("S0ix Transition Example with Platform Device, Procfs, and Sysfs");
   MODULE_LICENSE("GPL");
   MODULE_VERSION("1.0");

Error Codes and Return Values
-----------------------------

The module follows standard Linux kernel conventions for error reporting:

**Success values:**
- ``0`` - Operation completed successfully

**Common error codes:**
- ``-ENOMEM`` - Memory allocation failure
- ``-ENODEV`` - Device-related error  
- ``PTR_ERR(ptr)`` - Error encoded in pointer return

**Error handling strategy:**
- Graceful degradation where possible
- Complete cleanup on initialization failures
- Informative error messages via ``pr_err()``

Memory Management
-----------------

The module uses minimal dynamic memory allocation:

**Static allocation:**
- Statistics structure (``s_stats``)
- Mutex (``s0ix_device_lock``)
- Platform driver/device structures

**Dynamic allocation:**
- Platform device registration (handled by kernel)
- Proc/sysfs entries (handled by kernel)

**Cleanup:** All dynamically allocated resources are properly cleaned up in the exit function

Integration Points
------------------

Kernel Power Management
~~~~~~~~~~~~~~~~~~~~~~~

The module integrates with the Linux kernel power management subsystem through:

- Platform driver framework
- Device PM operations structure
- Standard suspend/resume callback mechanism

File System Integration
~~~~~~~~~~~~~~~~~~~~~~~

**Proc filesystem:**
- Single-open interface using ``seq_file`` operations
- Read-only access with proper permission settings

**Sysfs filesystem:**
- Device attribute groups
- Standard show function interface
- Automatic cleanup through device model

Build System Integration
~~~~~~~~~~~~~~~~~~~~~~~~

The module integrates with the kernel build system via:

- Kbuild makefile system
- Module metadata declarations
- Standard kernel module entry/exit points
