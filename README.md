# S0ix Power Mode Transition Driver

A Linux kernel driver that tracks system power mode transitions, specifically focusing on S0ix (Modern Standby) power states. This module provides detailed statistics about suspend/resume cycles and power management events.

## Features

- **Real-time tracking** of power management events
- **Suspend/resume cycle monitoring** with duration measurements
- **Statistics collection** including total suspend time and cycle counts
- **Proc filesystem interface** for easy data access
- **CMake build system** for streamlined compilation
- **Management script** for easy module operations

## Power Events Tracked

- `PM_SUSPEND_PREPARE` - System preparing for suspend
- `PM_POST_SUSPEND` - System resuming from suspend
- `PM_HIBERNATION_PREPARE` - System preparing for hibernation
- `PM_POST_HIBERNATION` - System resuming from hibernation
- `PM_RESTORE_PREPARE` - System preparing for restore
- `PM_POST_RESTORE` - System restored

## Quick Start

### Automated Setup (Recommended)

The easiest way to get started is using the provided setup script:

```bash
# Clone or navigate to the project directory
cd s0ix_driver

# Run the automated setup script
./setup_build_env.sh

# Build and load the module
make s0ix_driver_module
make s0ix_driver_load

# View statistics
make s0ix_driver_stats
```

The `setup_build_env.sh` script will:
- Detect your Linux distribution
- Install required dependencies automatically
- Verify kernel headers are available
- Configure the build environment with CMake
- Provide helpful status messages throughout the process

## Prerequisites

- Linux kernel headers for your current kernel version
- CMake 3.16 or later
- GCC compiler
- Root privileges for module loading/unloading

### Manual Installation of Prerequisites

If you prefer to install dependencies manually:

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install linux-headers-$(uname -r) cmake build-essential
```

#### RHEL/CentOS/Fedora
```bash
sudo dnf install kernel-devel kernel-headers cmake gcc make
# or for older systems:
sudo yum install kernel-devel kernel-headers cmake gcc make
```

## Building the Module

### Method 1: Using the Top-level Makefile (Recommended)

```bash
# Build the kernel module
make s0ix_driver_module

# Or build everything
make all
```

### Method 2: Using CMake directly

1. **Create build directory and configure**:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

2. **Build the kernel module**:
   ```bash
   make s0ix_driver_module
   ```

## Usage

### Using CMake Targets

```bash
# Build the module
make s0ix_driver_module

# Load the module
make s0ix_driver_load

# View statistics
make s0ix_driver_stats

# Unload the module
make s0ix_driver_unload

# Clean build files
make s0ix_driver_clean
```

### Using the Management Script

The build process creates a convenient management script:

```bash
# Load the module
./manage_module.sh load

# View statistics
./manage_module.sh stats

# Check module status
./manage_module.sh status

# Reload the module
./manage_module.sh reload

# Unload the module
./manage_module.sh unload

# Show help
./manage_module.sh help
```

### Manual Operations

```bash
# Load the module manually
sudo insmod s0ix_driver.ko

# View statistics
cat /proc/s0ix_stats

# Check kernel messages
dmesg | grep s0ix_driver

# Unload the module
sudo rmmod s0ix_driver
```

## Output Example

When you check `/proc/s0ix_stats`, you'll see output like this:

```
S0ix Power Mode Transition Driver
=====================================

Statistics:
  Total suspend cycles: 5
  Total resume events: 5
  Total suspend time: 12500 ms
  Average suspend time: 2500 ms
  Total transitions recorded: 10

Recent Transitions:
 Timestamp            Event                Duration (ms)  
 ---------            -----                -------------  
 1701234567.123       SUSPEND_PREPARE      0             
 1701234567.456       POST_SUSPEND         2500          
 1701234789.012       SUSPEND_PREPARE      0             
 1701234791.234       POST_SUSPEND         2222          
 1701234912.345       SUSPEND_PREPARE      0             
 1701234915.678       POST_SUSPEND         3333          
```

## Kernel Messages

The module logs important events to the kernel message buffer. View them with:

```bash
dmesg | grep s0ix_driver
```

Example output:
```
[12345.678901] s0ix_driver: Initializing S0ix driver module
[12345.678902] s0ix_driver: Module loaded successfully
[12345.678903] s0ix_driver: Statistics available at /proc/s0ix_stats
[12456.789012] s0ix_driver: Preparing for suspend
[12459.012345] s0ix_driver: Resumed from suspend (duration: 2223 ms)
```

## Testing

To test the module functionality:

1. **Load the module**:
   ```bash
   ./manage_module.sh load
   ```

2. **Trigger a suspend/resume cycle**:
   ```bash
   # Using systemctl (modern systems)
   sudo systemctl suspend
   
   # Or using pm-suspend (older systems)
   sudo pm-suspend
   
   # Or using echo to /sys/power/state
   echo mem | sudo tee /sys/power/state
   ```

3. **Check the statistics**:
   ```bash
   ./manage_module.sh stats
   ```

## Troubleshooting

### Module Won't Load

- **Check kernel headers**: Ensure you have the correct kernel headers installed
- **Check build errors**: Look for compilation errors in the build output
- **Check permissions**: Ensure you're running with sudo privileges
- **Check kernel version**: The module must be built for your current kernel

### No Statistics Showing

- **Verify module is loaded**: `lsmod | grep s0ix_driver`
- **Check proc entry**: `ls -la /proc/s0ix_stats`
- **Check kernel messages**: `dmesg | grep s0ix_driver`

### Build Fails

- **Run setup script**: Try `./setup_build_env.sh` to automatically install dependencies
- **Install kernel headers**: `sudo apt install linux-headers-$(uname -r)`
- **Check CMake version**: Requires CMake 3.16+
- **Check compiler**: Ensure GCC is installed
- **Check kernel build directory**: Ensure `/lib/modules/$(uname -r)/build` exists
- **Clean and rebuild**: `make clean && make s0ix_driver_module`

## File Structure

```
s0ix_driver/
├── build/                  # Build directory (created by CMake)
├── docs/                   # Documentation source (Sphinx/ReadtheDocs)
├── CMakeLists.txt          # Main CMake configuration
├── Kbuild.in              # Kernel build template
├── LICENSE                 # BSD 3-Clause License
├── Makefile               # Top-level Makefile for easy building
├── manage_module.sh.in     # Module management script template
├── README.md              # This file
├── s0ix_driver.c          # Main kernel module source
└── setup_build_env.sh     # Automated build environment setup script
```

### Generated Files (after build)
```
build/
├── Kbuild                 # Generated kernel build file
├── manage_module.sh       # Generated module management script
├── s0ix_driver.ko         # Compiled kernel module
└── ...                    # Other CMake build artifacts
```

## Recent Updates

### Version History

- **Latest**: Added automated build environment setup script
- **Enhanced**: Improved mutex synchronization in resume path
- **Added**: Top-level Makefile for easier building
- **Improved**: Better error handling and logging

## Technical Details

### Data Collection

- **Circular buffer**: Stores up to 1000 recent transitions
- **Thread-safe**: Uses mutex for concurrent access protection
- **High precision timing**: Uses `ktime_t` for accurate measurements
- **Memory efficient**: Dynamic allocation with proper cleanup
- **Synchronization**: Fixed mutex unlock timing to prevent race conditions

### Power Management Integration

- **PM notifier chain**: Registers with kernel power management subsystem
- **Event filtering**: Handles all major power management events
- **State tracking**: Maintains suspend/resume state consistency

### Proc Interface

- **Read-only access**: Statistics are read-only for security
- **Formatted output**: Human-readable statistics display
- **Real-time data**: Shows current statistics without caching

## Documentation

Comprehensive documentation is available in multiple formats:

### Online Documentation

The complete documentation is available on ReadtheDocs: [S0ix Driver Documentation](https://s0ix-driver.readthedocs.io) (replace with your actual ReadtheDocs URL)

### Building Documentation Locally

To build and view the documentation locally:

```bash
# Navigate to the docs directory
cd docs

# Build the documentation (creates virtual environment automatically)
./build_docs.sh

# View the documentation
open _build/html/index.html  # macOS
# or
xdg-open _build/html/index.html  # Linux
```

### Documentation Contents

The documentation includes:

- **Installation Guide** - Step-by-step setup instructions
- **Usage Examples** - Practical usage scenarios and commands
- **API Reference** - Detailed technical documentation
- **Troubleshooting** - Common issues and solutions
- **Development Guide** - Information for contributors
- **License Information** - Legal and licensing details

## License

This project is licensed under the BSD 3-Clause License. See the [LICENSE](LICENSE) file for details.

**Note:** While the project uses the BSD 3-Clause License, the kernel module itself must be GPL-compatible when loaded into the Linux kernel due to kernel licensing requirements. The MODULE_LICENSE declaration in the source code reflects this kernel requirement.

## Contributing

Contributions are welcome! Please ensure that:

- Code follows Linux kernel coding style
- All changes are tested on multiple kernel versions
- Documentation is updated accordingly
- Proper error handling is implemented

## Disclaimer

This is sample code for educational and development purposes. Use in production environments should be thoroughly tested and validated for your specific use case.

