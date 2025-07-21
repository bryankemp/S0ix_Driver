License
=======

The S0ix Driver is released under the BSD 3-Clause License, which is a permissive open source license that allows for both commercial and non-commercial use.

BSD 3-Clause License
--------------------

.. code-block:: text

   BSD 3-Clause License

   Copyright (c) 2024, Bryan Kemp
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

   3. Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

What This License Means
-----------------------

The BSD 3-Clause License is one of the most permissive open source licenses. It allows you to:

Permissions
~~~~~~~~~~~

- **Use** - You can use this software for any purpose, including commercial applications
- **Modify** - You can modify the source code to suit your needs
- **Distribute** - You can distribute the original or modified versions of the software
- **Sublicense** - You can incorporate this code into projects with different licenses
- **Commercial Use** - You can use this software in commercial products

Requirements
~~~~~~~~~~~~

When using or distributing this software, you must:

- **Include Copyright Notice** - Retain the original copyright notice in source distributions
- **Include License Text** - Include the full license text in source distributions
- **Include Disclaimer** - Include the disclaimer in binary distributions

Limitations
~~~~~~~~~~~

- **No Liability** - The authors are not liable for any damages resulting from use of this software
- **No Warranty** - The software is provided "as is" without any warranty
- **No Trademark Rights** - This license does not grant rights to use the authors' names or trademarks

Kernel Module Licensing Considerations
--------------------------------------

Important Note About GPL Compatibility
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While the S0ix Driver project uses the BSD 3-Clause License, there are special considerations for Linux kernel modules:

**Kernel Requirement:** The Linux kernel requires that loadable modules be GPL-compatible. This is enforced through the ``MODULE_LICENSE`` declaration in the kernel module code.

**Implementation:** In the source code, you'll see:

.. code-block:: c

   MODULE_LICENSE("GPL");

This declaration is required for the module to load into the Linux kernel and does not conflict with the BSD license for the project.

**Practical Impact:** 
- The project source code is available under BSD 3-Clause terms
- When loaded into the kernel, the module must comply with GPL requirements
- This allows maximum flexibility for the source code while ensuring kernel compatibility

License Compatibility
~~~~~~~~~~~~~~~~~~~~~

The BSD 3-Clause License is compatible with:

- **GPL (GNU General Public License)** - Can be incorporated into GPL projects
- **MIT License** - Similar permissiveness level
- **Apache License 2.0** - Compatible for most uses
- **Proprietary licenses** - Can be incorporated into closed-source projects

This compatibility means you can freely incorporate S0ix Driver code into projects with different licensing terms.

Third-Party Components
----------------------

Build System
~~~~~~~~~~~~~

The project uses several build system components:

- **CMake** - Cross-platform build system (BSD-style license)
- **Linux Kernel Build System** - Part of the Linux kernel (GPL)
- **GNU Make** - Build automation tool (GPL)

These tools are used during the build process but are not distributed with the S0ix Driver source code.

Documentation Tools
~~~~~~~~~~~~~~~~~~~~

Documentation is built using:

- **Sphinx** - Documentation generator (BSD-style license)
- **ReadtheDocs Theme** - Documentation theme (MIT License)
- **Breathe** - Doxygen integration for Sphinx (BSD-style license)

These tools are specified in ``docs/requirements.txt`` for documentation building.

Contributing
------------

Contributor License
~~~~~~~~~~~~~~~~~~~

By contributing to the S0ix Driver project, you agree that your contributions will be licensed under the same BSD 3-Clause License that covers the project.

**What this means:**
- Your contributions become part of the BSD 3-Clause licensed codebase
- Others can use your contributions under the same permissive terms
- You retain copyright to your contributions
- You grant rights to use your contributions consistent with the project license

Copyright Attribution
~~~~~~~~~~~~~~~~~~~~~

When making significant contributions, you may add your copyright notice to the relevant files:

.. code-block:: c

   /*
    * Copyright (c) 2024, Bryan Kemp
    * Copyright (c) 2024, Your Name <your.email@example.com>
    * All rights reserved.
    * ...
    */

Signed-off-by Process
~~~~~~~~~~~~~~~~~~~~~

For contributions, please include a "Signed-off-by" line in your commit messages:

.. code-block:: text

   Signed-off-by: Your Name <your.email@example.com>

This indicates that you certify the contribution under the Developer Certificate of Origin.

Commercial Use
--------------

The BSD 3-Clause License explicitly permits commercial use:

**Allowed Commercial Activities:**
- Including the driver in commercial Linux distributions
- Incorporating the code into proprietary kernel modules
- Using the driver in commercial testing or development environments
- Modifying the driver for commercial products

**Requirements for Commercial Use:**
- Include the original license and copyright notice
- Include the BSD license text in your documentation
- Do not use the original author's name for endorsement without permission

**No Royalties or Fees:**
- No payment is required for commercial use
- No ongoing royalties or licensing fees
- No registration or notification required

Support and Warranty Disclaimer
-------------------------------

No Warranty
~~~~~~~~~~~

The software is provided "AS IS" without warranty of any kind. This means:

- No guarantee that the software will work for your specific use case
- No guarantee that the software is free from bugs or security vulnerabilities  
- No guarantee of ongoing support or updates
- No liability for damages resulting from use of the software

Community Support
~~~~~~~~~~~~~~~~~

While there's no warranty, the project aims to provide:

- Documentation and usage examples
- Issue tracking for bug reports
- Community-driven support through forums or issue trackers
- Best-effort responses to questions and problems

**Note:** This is community support, not a commercial support obligation.

License Questions
-----------------

If you have questions about the license terms or how they apply to your use case:

1. **Review the License Text** - The full BSD 3-Clause license text above contains the complete terms
2. **Consult Legal Advice** - For complex commercial or legal questions, consult with a qualified attorney
3. **Check Compatibility** - Verify compatibility with your project's existing licenses
4. **Community Resources** - Open source license compatibility information is available from organizations like the FSF and OSI

**Disclaimer:** This documentation provides general information about the license but is not legal advice. The license text itself is the authoritative source for all legal terms and conditions.
