## FreeRTOS+POSIX: Portable Operating System Interface (POSIX threading wrapper) for FreeRTOS

The Portable Operating System Interface (POSIX) is a family of standards specified by the IEEE Computer Society for maintaining compatibility between operating systems. FreeRTOS+POSIX implements a subset of the [POSIX threading](https://pubs.opengroup.org/onlinepubs/7908799/xsh/threads.html) API.

With FreeRTOS+POSIX, an existing POSIX threading compliant application can be ported to run on the FreeRTOS kernel, and therefor leverage all the [Amazon FreeRTOS](https://www.freertos.org/FAQ_Amazon.html) functionality. Additionally, a library designed for use with POSIX threading compatible operating systems can be ported to FreeRTOS kernel based applications.

For more on FreeRTOS+POSIX, please visit [freertos.org](https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_POSIX/index.html)

This repository only contains source code. For demo applications, please visit https://github.com/FreeRTOS/FreeRTOS-Labs.

## To consume FreeRTOS+POSIX
It is recommended to use this repository as a submodule. Please refer to [Git Tools — Submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

## Notes
This project is undergoing optimizations or refactorization to improve memory usage, modularity, documentation, demo usability, or test coverage.
