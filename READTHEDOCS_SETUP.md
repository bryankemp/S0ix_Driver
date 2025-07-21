# ReadtheDocs Setup Instructions

## Step 1: Create ReadtheDocs Account
1. Go to https://readthedocs.org
2. Click "Sign up" in the top right
3. Choose "Sign up with GitHub" for easy integration
4. Authorize ReadtheDocs to access your GitHub repositories

## Step 2: Import Your Project
1. Once logged in, click "Import a Project"
2. You should see your `S0ix_Driver` repository in the list
3. Click the "+" button next to your repository
4. Fill in the project details:
   - **Name**: `S0ix Driver` (or your preference)
   - **Repository URL**: Should be auto-filled
   - **Repository type**: Git
   - **Description**: `Linux kernel module for tracking S0ix power states`
   - **Language**: English
   - **Programming Language**: C
   - **Project homepage**: Your GitHub repository URL
   - **Tags**: `kernel`, `linux`, `power-management`, `s0ix`

## Step 3: Configure Build Settings
1. After importing, go to your project's admin panel
2. Click on "Advanced Settings"
3. Verify these settings:
   - **Default branch**: `main`
   - **Default version**: `latest`
   - **Documentation type**: `Sphinx Html`
   - **Requirements file**: `docs/requirements.txt`
   - **Python configuration file**: `docs/conf.py`

## Step 4: Trigger First Build
1. Go to "Builds" in your project dashboard
2. Click "Build Version" to trigger a manual build
3. Monitor the build log for any errors

## Expected Build Process
ReadtheDocs will automatically:
1. Clone your repository
2. Create a Python 3.11 environment (as specified in .readthedocs.yaml)
3. Install system dependencies (doxygen, graphviz)
4. Install Python dependencies from docs/requirements.txt
5. Run Sphinx to build HTML and PDF documentation
6. Deploy to your project URL

## Your Documentation URL
Once built, your documentation will be available at:
`https://s0ix-driver.readthedocs.io/en/latest/`
(Replace `s0ix-driver` with your actual project slug)

## Troubleshooting
- If build fails, check the build logs in ReadtheDocs dashboard
- Verify all files are pushed to GitHub
- Ensure .readthedocs.yaml is in the repository root
- Check that docs/requirements.txt contains all needed packages

## Automatic Updates
- ReadtheDocs will automatically rebuild when you push to GitHub
- Multiple versions supported (main branch = latest)
- Pull request builds available (if enabled)
