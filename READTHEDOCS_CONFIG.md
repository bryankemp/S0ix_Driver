# ReadtheDocs Import Configuration

## Project Import Settings

When importing your project to ReadtheDocs, use these exact settings:

### Basic Information
- **Name**: `S0ix Driver`
- **Repository URL**: `https://github.com/bryankemp/S0ix_Driver`
- **Repository type**: `Git`
- **Default branch**: `main`
- **Language**: `English`
- **Programming Language**: `C`

### Project Details
- **Description**: `Linux kernel module for tracking S0ix power states with comprehensive documentation`
- **Project homepage**: `https://github.com/bryankemp/S0ix_Driver`
- **Tags**: Add these tags separated by commas:
  ```
  kernel, linux, power-management, s0ix, driver, documentation
  ```

### Advanced Settings (After Import)
Go to Admin → Advanced Settings and verify:

- **Default version**: `latest`
- **Default branch**: `main`
- **Documentation type**: `Sphinx Html` (should auto-detect)
- **Requirements file**: `docs/requirements.txt` (should auto-detect)
- **Python configuration file**: `docs/conf.py` (should auto-detect)
- **Use system packages**: ✅ Enabled (for doxygen, graphviz)

## What ReadtheDocs Will Do Automatically

1. **Clone your repository** from GitHub
2. **Read .readthedocs.yaml** configuration file
3. **Create Python 3.11 environment**
4. **Install system packages**: doxygen, graphviz
5. **Install Python packages** from docs/requirements.txt:
   - sphinx>=5.0
   - sphinx-rtd-theme>=1.0
   - breathe>=4.34.0
   - sphinx-c-autodoc>=1.4.0
   - myst-parser>=0.18.0
6. **Build documentation** with Sphinx
7. **Generate HTML, PDF, and ePub** formats
8. **Deploy to your project URL**

## Expected Build Time
- First build: 2-5 minutes
- Subsequent builds: 1-2 minutes

## Your Documentation URLs
Once built successfully:
- **Main docs**: `https://s0ix-driver.readthedocs.io/en/latest/`
- **PDF version**: `https://s0ix-driver.readthedocs.io/_/downloads/en/latest/pdf/`
- **ePub version**: `https://s0ix-driver.readthedocs.io/_/downloads/en/latest/epub/`

## Troubleshooting Common Issues

### Build Fails with "No module named 'breathe'"
- Check that `docs/requirements.txt` exists and contains all packages
- Verify `.readthedocs.yaml` is in repository root

### Build Fails with "Sphinx error"
- Check `docs/conf.py` syntax
- Verify all `.rst` files have correct reStructuredText syntax

### Build Succeeds but Pages are Missing
- Check that all documentation files are committed to GitHub
- Verify `docs/index.rst` contains correct `toctree` entries

### Authentication Issues
- Ensure repository is Public on GitHub (required for free ReadtheDocs)
- Check that ReadtheDocs has permission to access your repositories
