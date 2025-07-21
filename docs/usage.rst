Usage
=====

This guide covers how to use the S0ix Driver once it's installed and loaded into the kernel.

Basic Operations
----------------

The S0ix Driver provides several interfaces for interacting with the module and viewing statistics.

Loading and Unloading
~~~~~~~~~~~~~~~~~~~~~~

Using CMake Targets
^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   # Load the module
   make s0ix_driver_load

   # Unload the module  
   make s0ix_driver_unload

   # View current statistics
   make s0ix_driver_stats

Using the Management Script
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The build process creates a convenient management script with colored output:

.. code-block:: bash

   # Load the module
   ./build/manage_module.sh load

   # Check module status
   ./build/manage_module.sh status

   # View statistics
   ./build/manage_module.sh stats

   # Reload the module (unload then load)
   ./build/manage_module.sh reload

   # Unload the module
   ./build/manage_module.sh unload

   # Show help
   ./build/manage_module.sh help

Manual Operations
^^^^^^^^^^^^^^^^^

.. code-block:: bash

   # Load the module manually
   sudo insmod build/s0ix_driver.ko

   # Check if module is loaded
   lsmod | grep s0ix_driver

   # Unload the module manually
   sudo rmmod s0ix_driver

Viewing Statistics
------------------

The S0ix Driver provides statistics through multiple interfaces:

Proc Filesystem Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~

The primary interface is through ``/proc/s0ix_stats``:

.. code-block:: bash

   # View current statistics
   cat /proc/s0ix_stats

Example output:

.. code-block:: text

   S0ix Power Mode Transition Driver
   =====================================

   Statistics:
     Total suspend cycles: 5
     Total resume events: 5  
     Total suspend time: 12500 ms
     Average suspend time: 2500 ms

Sysfs Interface
~~~~~~~~~~~~~~~

Individual statistics are available through sysfs attributes:

.. code-block:: bash

   # View suspend count
   cat /sys/devices/platform/s0ix_device/suspend_count

   # View resume count  
   cat /sys/devices/platform/s0ix_device/resume_count

   # View total suspend time
   cat /sys/devices/platform/s0ix_device/total_suspend_time

   # View average suspend time
   cat /sys/devices/platform/s0ix_device/avg_suspend_time

Available sysfs attributes:

- ``suspend_count`` - Number of suspend operations initiated
- ``resume_count`` - Number of resume operations completed  
- ``total_suspend_time`` - Cumulative time spent suspended (milliseconds)
- ``avg_suspend_time`` - Average suspend duration (milliseconds)

Kernel Log Messages
~~~~~~~~~~~~~~~~~~~

Real-time events are logged to the kernel message buffer:

.. code-block:: bash

   # View S0ix driver messages
   dmesg | grep s0ix_driver

   # Follow messages in real-time
   dmesg -w | grep s0ix_driver

Example log output:

.. code-block:: text

   [12345.678901] s0ix_driver: Platform driver, device, proc, and sysfs registered
   [12456.789012] s0ix_device: Entering S0ix
   [12459.012345] s0ix_device: Exiting S0ix (last suspend: 2223 ms)
   [12470.123456] s0ix_device: Driver enabled after 11 seconds.

Testing the Module
------------------

To verify the module is working correctly, you can trigger suspend/resume cycles:

Triggering Suspend Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Modern Systems (systemd)
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   # Suspend using systemctl
   sudo systemctl suspend

Traditional Methods
^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   # Suspend using pm-suspend
   sudo pm-suspend

   # Suspend using direct sysfs interface
   echo mem | sudo tee /sys/power/state

   # For systems supporting S0ix specifically
   echo s2idle | sudo tee /sys/power/mem_sleep
   echo mem | sudo tee /sys/power/state

Testing Workflow
~~~~~~~~~~~~~~~~

1. **Load the module:**

   .. code-block:: bash

      ./build/manage_module.sh load

2. **Check initial statistics:**

   .. code-block:: bash

      ./build/manage_module.sh stats

3. **Trigger a suspend/resume cycle:**

   .. code-block:: bash

      sudo systemctl suspend

   *Wake the system after a few seconds using keyboard/mouse input*

4. **Check updated statistics:**

   .. code-block:: bash

      ./build/manage_module.sh stats

5. **View kernel messages:**

   .. code-block:: bash

      dmesg | grep s0ix_driver | tail

Monitoring and Analysis
-----------------------

Long-term Monitoring
~~~~~~~~~~~~~~~~~~~~

For continuous monitoring, you can use scripts to periodically collect statistics:

.. code-block:: bash

   #!/bin/bash
   # Simple monitoring script
   while true; do
       echo "$(date): $(cat /proc/s0ix_stats | grep 'Total suspend cycles')"
       sleep 60
   done

Automated Data Collection
~~~~~~~~~~~~~~~~~~~~~~~~~

Collect data programmatically using sysfs:

.. code-block:: bash

   #!/bin/bash
   # Data collection script
   TIMESTAMP=$(date +%s)
   SUSPEND_COUNT=$(cat /sys/devices/platform/s0ix_device/suspend_count)
   RESUME_COUNT=$(cat /sys/devices/platform/s0ix_device/resume_count)  
   TOTAL_TIME=$(cat /sys/devices/platform/s0ix_device/total_suspend_time)
   AVG_TIME=$(cat /sys/devices/platform/s0ix_device/avg_suspend_time)

   echo "$TIMESTAMP,$SUSPEND_COUNT,$RESUME_COUNT,$TOTAL_TIME,$AVG_TIME"

Power Management Analysis
~~~~~~~~~~~~~~~~~~~~~~~~~

The statistics can help analyze power management behavior:

- **Frequent short suspends**: May indicate system or application issues
- **Long average suspend times**: Could suggest slow resume processes  
- **Mismatched suspend/resume counts**: Might indicate crashed resume operations
- **Growing total suspend time**: Shows cumulative power saving potential

Integration with System Tools
------------------------------

Systemd Service Integration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Create a systemd service to automatically load the module at boot:

.. code-block:: bash

   # Create service file
   sudo tee /etc/systemd/system/s0ix-driver.service << EOF
   [Unit]
   Description=S0ix Driver Kernel Module
   After=multi-user.target

   [Service]
   Type=oneshot
   ExecStart=/sbin/insmod /path/to/s0ix_driver.ko
   ExecStop=/sbin/rmmod s0ix_driver
   RemainAfterExit=yes

   [Install]
   WantedBy=multi-user.target
   EOF

   # Enable the service
   sudo systemctl enable s0ix-driver.service

Cron Integration
~~~~~~~~~~~~~~~~

Schedule periodic statistics collection:

.. code-block:: bash

   # Add to crontab
   */5 * * * * cat /proc/s0ix_stats >> /var/log/s0ix_stats.log

Log Analysis
~~~~~~~~~~~~

Parse and analyze collected data:

.. code-block:: bash

   # Extract suspend counts over time
   grep "Total suspend cycles" /var/log/s0ix_stats.log

   # Calculate power management frequency  
   awk '/Total suspend cycles:/ { print $4 }' /var/log/s0ix_stats.log

Best Practices
--------------

- **Regular Monitoring**: Check statistics periodically to identify patterns
- **Clean Unloading**: Always unload the module cleanly to avoid resource leaks
- **Log Rotation**: Implement log rotation for long-term monitoring setups  
- **Testing Environment**: Use in development/testing environments initially
- **Documentation**: Document any custom integration or automation scripts
