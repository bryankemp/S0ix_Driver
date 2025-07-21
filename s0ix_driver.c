/*
 * S0ix Power Mode Transition Driver
 *
 * Copyright (c) 2024, Bryan Kemp
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/timekeeping.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/delay.h>

#define S0IX_DEVICE_NAME "s0ix_device"
#define PROC_STATS_NAME "s0ix_stats"

// Mutex to protect access to stats and shared state
static DEFINE_MUTEX(s0ix_device_lock);

/* Statistics structure for tracking suspend/resume events and timings */
struct s0ix_stats {
    unsigned long suspend_count;        // Number of suspend operations
    unsigned long resume_count;         // Number of resume operations
    u64 total_suspend_time_ms;          // Total time spent suspended (in ms)
    ktime_t last_suspend_time;          // Timestamp when last suspend started
};

// Global instance for statistics
static struct s0ix_stats s_stats;

// Pointer to the registered platform device
static struct platform_device *s0ix_platform_device;

/*
 * Sysfs attribute show functions. 
 * These allow reading of statistics via /sys/class/s0ix_device/...
 */
static ssize_t suspend_count_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
    ssize_t ret;
    mutex_lock(&s0ix_device_lock);
    ret = scnprintf(buf, PAGE_SIZE, "%lu\n", s_stats.suspend_count);
    mutex_unlock(&s0ix_device_lock);
    return ret;
}
static DEVICE_ATTR_RO(suspend_count);

static ssize_t resume_count_show(struct device *dev,
                                 struct device_attribute *attr, char *buf)
{
    ssize_t ret;
    mutex_lock(&s0ix_device_lock);
    ret = scnprintf(buf, PAGE_SIZE, "%lu\n", s_stats.resume_count);
    mutex_unlock(&s0ix_device_lock);
    return ret;
}
static DEVICE_ATTR_RO(resume_count);

static ssize_t avg_suspend_time_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
    ssize_t ret;
    u64 avg = 0;
    mutex_lock(&s0ix_device_lock);
    if (s_stats.resume_count)
        avg = s_stats.total_suspend_time_ms / s_stats.resume_count;
    ret = scnprintf(buf, PAGE_SIZE, "%llu\n", avg);
    mutex_unlock(&s0ix_device_lock);
    return ret;
}
static DEVICE_ATTR_RO(avg_suspend_time);

static ssize_t total_suspend_time_show(struct device *dev,
                                       struct device_attribute *attr, char *buf)
{
    ssize_t ret;
    mutex_lock(&s0ix_device_lock);
    ret = scnprintf(buf, PAGE_SIZE, "%llu\n", s_stats.total_suspend_time_ms);
    mutex_unlock(&s0ix_device_lock);
    return ret;
}
static DEVICE_ATTR_RO(total_suspend_time);

// List of sysfs attributes presented by the device
static struct attribute *s0ix_attrs[] = {
    &dev_attr_suspend_count.attr,
    &dev_attr_resume_count.attr,
    &dev_attr_total_suspend_time.attr,
    &dev_attr_avg_suspend_time.attr,
    NULL
};

// Attribute group for sysfs
static const struct attribute_group s0ix_attr_group = {
    .attrs = s0ix_attrs,
};

/* 
 * Power management callbacks 
 */
static int s0ix_device_suspend(struct device *dev)
{
    // Lock to protect stats update
    mutex_lock(&s0ix_device_lock);
    s_stats.suspend_count++;            // Increment suspend counter
    s_stats.last_suspend_time = ktime_get(); // Record suspend start time
    pr_info("%s: Entering S0ix\n", S0IX_DEVICE_NAME);
    mutex_unlock(&s0ix_device_lock);
    return 0;
}

static int s0ix_device_resume(struct device *dev)
{
    ktime_t now;
    u64 duration_ms = 0;

    // Lock to ensure exclusive access to stats
    mutex_lock(&s0ix_device_lock);

    s_stats.resume_count++;                 // Increment resume counter
    now = ktime_get();                      // Fetch current time

    // Calculate how long the device was suspended (in ms)
    if (s_stats.last_suspend_time)
        duration_ms = ktime_to_ms(ktime_sub(now, s_stats.last_suspend_time));

    s_stats.total_suspend_time_ms += duration_ms;   // Update total suspend time

    // Log the duration in kernel logs
    pr_info("%s: Exiting S0ix (last suspend: %llu ms)\n", S0IX_DEVICE_NAME, duration_ms);

    // Pause execution for 11 seconds (11,000 ms) to simulate processing delay after resume
    // msleep() will put the current process to sleep, allowing other processes to run
    msleep(11000);
    pr_info("%s: Driver enabled after 11 seconds.\n", S0IX_DEVICE_NAME);

    mutex_unlock(&s0ix_device_lock);
    return 0;
}

// Device power management operations structure
static const struct dev_pm_ops s0ix_pm_ops = {
    .suspend = s0ix_device_suspend,
    .resume  = s0ix_device_resume,
};

// Platform driver structure
static struct platform_driver s0ix_platform_driver = {
    .driver = {
        .name = S0IX_DEVICE_NAME,
        .owner = THIS_MODULE,
        .pm = &s0ix_pm_ops,
    },
};

/* 
 * Procfs implementation for statistics reporting 
 */

// seq_file show callback: prints all statistics to the proc entry
static int s0ix_stats_show(struct seq_file *m, void *v)
{
    unsigned long suspend_count, resume_count;
    u64 total_suspend_time_ms;
    u64 avg_suspend_time = 0;

    // Lock to safely snapshot stats
    mutex_lock(&s0ix_device_lock);
    suspend_count = s_stats.suspend_count;
    resume_count = s_stats.resume_count;
    total_suspend_time_ms = s_stats.total_suspend_time_ms;
    if (resume_count)
        avg_suspend_time = total_suspend_time_ms / resume_count;
    mutex_unlock(&s0ix_device_lock);

    // Print statistics summary to procfs
    seq_printf(m,
        "S0ix Power Mode Transition Driver\n"
        "=====================================\n\n"
        "Statistics:\n"
        "  Total suspend cycles: %lu\n"
        "  Total resume events: %lu\n"
        "  Total suspend time: %llu ms\n"
        "  Average suspend time: %llu ms\n",
        suspend_count,
        resume_count,
        total_suspend_time_ms,
        avg_suspend_time);

    return 0;
}

// Open callback for single_open wrapper
static int s0ix_stats_open(struct inode *inode, struct file *file)
{
    return single_open(file, s0ix_stats_show, NULL);
}

// Procfs file operations structure
static const struct proc_ops s0ix_stats_proc_ops = {
    .proc_open    = s0ix_stats_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

// Pointer to the procfs entry for statistics
static struct proc_dir_entry *s0ix_proc_entry;

/* 
 * Module initialization: sets up platform driver/device, sysfs, and procfs 
 */
static int __init s0ix_driver_init(void)
{
    int ret;

    /* Reset all statistics to zero at load */
    memset(&s_stats, 0, sizeof(s_stats));

    // Register the platform driver
    ret = platform_driver_register(&s0ix_platform_driver);
    if (ret) {
        pr_err("%s: Failed to register platform driver: %d\n", S0IX_DEVICE_NAME, ret);
        return ret;
    }

    // Register a simple platform device
    s0ix_platform_device = platform_device_register_simple(S0IX_DEVICE_NAME, -1, NULL, 0);
    if (IS_ERR(s0ix_platform_device)) {
        pr_err("%s: Failed to register platform device\n", S0IX_DEVICE_NAME);
        platform_driver_unregister(&s0ix_platform_driver);
        return PTR_ERR(s0ix_platform_device);
    }

    /* Create a sysfs group for all the attribute files */
    ret = sysfs_create_group(&s0ix_platform_device->dev.kobj, &s0ix_attr_group);
    if (ret) {
        pr_err("%s: Failed to create sysfs group\n", S0IX_DEVICE_NAME);
        platform_device_unregister(s0ix_platform_device);
        platform_driver_unregister(&s0ix_platform_driver);
        return ret;
    }

    /* Create a procfs entry for statistics reporting */
    s0ix_proc_entry = proc_create(PROC_STATS_NAME, 0444, NULL, &s0ix_stats_proc_ops);
    if (!s0ix_proc_entry) {
        pr_err("%s: Failed to create /proc/%s\n", S0IX_DEVICE_NAME, PROC_STATS_NAME);
        sysfs_remove_group(&s0ix_platform_device->dev.kobj, &s0ix_attr_group);
        platform_device_unregister(s0ix_platform_device);
        platform_driver_unregister(&s0ix_platform_driver);
        return -ENOMEM;
    }

    pr_info("%s: Platform driver, device, proc, and sysfs registered\n", S0IX_DEVICE_NAME);
    return 0;
}

/* 
 * Module exit: cleanup all resources 
 */
static void __exit s0ix_driver_exit(void)
{
    if (s0ix_proc_entry)
        proc_remove(s0ix_proc_entry);

    if (s0ix_platform_device)
        sysfs_remove_group(&s0ix_platform_device->dev.kobj, &s0ix_attr_group);

    if (s0ix_platform_device)
        platform_device_unregister(s0ix_platform_device);

    platform_driver_unregister(&s0ix_platform_driver);
    pr_info("%s: Cleaned up module\n", S0IX_DEVICE_NAME);
}

// Register the module's init and exit routines
module_init(s0ix_driver_init);
module_exit(s0ix_driver_exit);

// Module information
MODULE_AUTHOR("Bryan Kemp");
MODULE_DESCRIPTION("S0ix Transition Example with Platform Device, Procfs, and Sysfs");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");