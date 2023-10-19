/*
 * FreeRTOS Kernel V10.4.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef __ERRNO_H__
#define __ERRNO_H__

#define NONE             0   /* No errors */
#define ENOENT           2   /* No such file or directory */
#define EINTR            4   /* Interrupted system call */
#define EIO              5   /* I/O error */
#define ENXIO            6   /* No such device or address */
#define EBADF            9   /* Bad file number */
#define EAGAIN           11  /* No more processes */
#define EWOULDBLOCK      11  /* Operation would block */
#define ENOMEM           12  /* Not enough memory */
#define EACCES           13  /* Permission denied */
#define EFAULT           14  /* Bad address */
#define EBUSY            16  /* Mount device busy */
#define EEXIST           17  /* File exists */
#define EXDEV            18  /* Cross-device link */
#define ENODEV           19  /* No such device */
#define ENOTDIR          20  /* Not a directory */
#define EISDIR           21  /* Is a directory */
#define EINVAL           22  /* Invalid argument */
#define ENOSPC           28  /* No space left on device */
#define ESPIPE           29  /* Illegal seek */
#define EROFS            30  /* Read only file system */
#define EUNATCH          42  /* Protocol driver not attached */
#define EBADE            50  /* Invalid exchange */
#define EFTYPE           79  /* Inappropriate file type or format */
#define ENMFILE          89  /* No more files */
#define ENOTEMPTY        90  /* Directory not empty */
#define ENAMETOOLONG     91  /* File or path name too long */
#define EOPNOTSUPP       95  /* Operation not supported on transport endpoint */
#define ENOBUFS          105 /* No buffer space available */
#define ENOPROTOOPT      109 /* Protocol not available */
#define EADDRINUSE       112 /* Address already in use */
#define ETIMEDOUT        116 /* Connection timed out */
#define EINPROGRESS      119 /* Connection already in progress */
#define EALREADY         120 /* Socket already connected */
#define EADDRNOTAVAIL    125 /* Address not available */
#define EISCONN          127 /* Socket is already connected */
#define ENOTCONN         128 /* Socket is not connected */
#define ENOMEDIUM        135 /* No medium inserted */
#define EILSEQ           138 /* An invalid UTF-16 sequence was encountered. */
#define ECANCELED        140 /* Operation canceled. */

#define ERESTARTSYS 512
#define ERESTARTNOINTR  513
#define ERESTARTNOHAND  514 /* restart if no handler.. */
#define ENOIOCTLCMD 515 /* No ioctl command */
#define ERESTART_RESTARTBLOCK 516 /* restart by calling sys_restart_syscall */
#define EPROBE_DEFER    517 /* Driver requests probe retry */
#define EOPENSTALE  518 /* open found a stale dentry */

/* move from uapi/asm-generic/errno-base.h */
#define EPERM        1  /* Operation not permitted */
#define ENOTTY          25      /* Not a typewriter */

/* move from uapi/asm-generic/errno.h */
#define ETIME       62  /* Timer expired */

/* move from uapi/asm-generic/errno.h*/
#define	ENODATA		61	/* No data available */

/* move from uapi/asm-generic/errno-base.h*/
#define EPIPE           32      /* Broken pipe */

#endif /* __ERRNO_H__ */
