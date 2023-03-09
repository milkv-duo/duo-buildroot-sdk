#ifndef __CONFIG_H__
#define __CONFIG_H__

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN32) || defined(__MINGW32__)
#	define PLATFORM_WIN32
#elif defined(linux) || defined(__linux) || defined(ANDROID)
#	define PLATFORM_LINUX
#else
#	define PLATFORM_NON_OS
#endif

#if defined(_MSC_VER)
#	include <windows.h>
#	include <conio.h>
#	define inline _inline
#	define VPU_DELAY_MS(X) Sleep(X)
//should change to delay function which can be delay a microsecond unut.
#	define VPU_DELAY_US(X) Sleep(X)
#	define kbhit _kbhit
#	define getch _getch
#elif defined(__GNUC__)
#ifdef	_KERNEL_
#	define VPU_DELAY_MS(X)		udelay((X) * 1000)
#	define VPU_DELAY_US(X)		udelay(X)
#else
#	define VPU_DELAY_MS(X)		usleep((X) * 1000)
#	define VPU_DELAY_US(X)		usleep(X)
#endif
#elif defined(__ARMCC__)
#else
#  error "Unknown compiler."
#endif

#define PROJECT_ROOT	"..\\..\\..\\"

#if defined(JPU_FPGA_PLATFORM)
#if defined(ANDROID) || defined(linux)
#else
#define SUPPORT_CONF_TEST
#endif
#endif

#define API_VERSION 165

#define HAVE_STDIN_H 1
//#define MJPEG_ERROR_CONCEAL

#endif	/* __CONFIG_H__ */
