FSBL
====

FSBL is First Stage Boot Loader.
Act as ATF BL2.

License
=======

Project license: GPL-2.0-or-later (SPDX: GPL-2.0-or-later).

Unless stated otherwise at file level, original code authored by Sophgo
Technologies in this repository is licensed under GPL-2.0-or-later
(see LICENSE at repository root).

This project contains code derived from U-Boot (SPDX: GPL-2.0+) which is
compiled into the FSBL image. The project as a whole is therefore
distributed under the GPL; BSD licensing is not applicable.

Third-party components
----------------------

- ARM Trusted Firmware (TF-A) derived code
  Copyright (c) 2013-2017, ARM Limited and Contributors
  License: BSD-3-Clause (SPDX: BSD-3-Clause)

- U-Boot derived code: Designware DWC2 USB device controller
  plat/cv180x/usb/dwc2_udc_otg*.c
  plat/cv180x/include/usb/dwc2_udc*.h
  Copyright (C) 2008 Samsung Electronics, ported to U-Boot by
  Marek Szyprowski and Lukasz Majewski
  License: GPL-2.0-or-later (SPDX: GPL-2.0+)

- lib/crc/crc16.c
  License: eCos-2.0 (GPL-2.0 with linking exception)

- lib/libtommath/
  License: Unlicense (public domain)

- lib/libtomcrypt/
  License: public domain

- lib/BigDigits/
  License: MPL-2.0 (see lib/BigDigits/LICENSE.txt)

- lib/lz4/
  License: BSD-2-Clause

- lib/lzma/
  License: public domain

File-level SPDX headers remain authoritative for each individual file.
