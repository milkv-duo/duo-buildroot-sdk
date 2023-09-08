/*
	Copyright (c) 2016 CurlyMo <curlymoo1@gmail.com>

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _WIRING_X_H_
#define _WIRING_X_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <syslog.h>

#define wiringXLog(a, b, ...) _wiringXLog(a, __FILE__, __LINE__, b, ##__VA_ARGS__)

extern void (*_wiringXLog)(int, char *, int, const char *, ...);

#define EXPORT __attribute__((visibility("default")))

#if !defined(PATH_MAX)
    #if defined(_POSIX_PATH_MAX)
        #define PATH_MAX _POSIX_PATH_MAX
    #else
        #define PATH_MAX 1024
    #endif
#endif

enum function_t {
	FUNCTION_UNKNOWN = 0,
	FUNCTION_DIGITAL = 2,
	FUNCTION_ANALOG = 4,
	FUNCTION_I2C = 16,
	FUNCTION_INTERRUPT = 32
};

enum pinmode_t {
	PINMODE_NOT_SET = 0,
	PINMODE_INPUT = 2,
	PINMODE_OUTPUT = 4,
	PINMODE_INTERRUPT = 8
};

enum isr_mode_t {
	ISR_MODE_UNKNOWN = 0,
	ISR_MODE_RISING = 2,
	ISR_MODE_FALLING = 4,
	ISR_MODE_BOTH = 8,
	ISR_MODE_NONE = 16
};

enum digital_value_t {
	LOW,
	HIGH
};

typedef struct wiringXSerial_t {
	unsigned int baud;
	unsigned int databits;
	unsigned int parity;
	unsigned int stopbits;
	unsigned int flowcontrol;
} wiringXSerial_t;


int wiringXSetup(char *name, void (*func)(int, char *, int, const char *, ...));
int wiringXGC(void);

void delayMicroseconds(unsigned int howLong);

int wiringXValidGPIO(int pin);

int pinMode(int pin, enum pinmode_t mode);
int digitalWrite(int pin, enum digital_value_t value);
int digitalRead(int pin);

int waitForInterrupt(int pin, int ms);
int wiringXISR(int pin, enum isr_mode_t mode);

// int analogRead(int channel);

int wiringXI2CRead(int fd);
int wiringXI2CReadReg8(int fd, int reg);
int wiringXI2CReadReg16(int fd, int reg);
int wiringXI2CWrite(int fd, int data);
int wiringXI2CWriteReg8(int fd, int reg, int data);
int wiringXI2CWriteReg16(int fd, int reg, int data);
int wiringXI2CSetup(const char *path, int addr);

int wiringXSPIGetFd(int channel);
int wiringXSPIDataRW(int channel, unsigned char *data, int len);
int wiringXSPISetup(int channel, int speed);

int wiringXSerialOpen(const char *device, struct wiringXSerial_t serial);
void wiringXSerialFlush(int fd);
void wiringXSerialClose(int fd);
void wiringXSerialPutChar(int fd, unsigned char c);
void wiringXSerialPuts(int fd, const char *s);
void wiringXSerialPrintf(int fd, const char *message, ...);
int wiringXSerialDataAvail(int fd);
int wiringXSerialGetChar(int fd);

int wiringXSetPWMPeriod(int pin, long period);
int wiringXSetPWMPolarity(int pin, int polarity);
int wiringXSetPWMDuty(int pin, int duty_cycle);
int wiringXPWMEnable(int pin, int enable);

char *wiringXPlatform(void);
int wiringXSelectableFd(int gpio);
int wiringXSupportedPlatforms(char ***out);

#ifdef __cplusplus
}
#endif

#endif
