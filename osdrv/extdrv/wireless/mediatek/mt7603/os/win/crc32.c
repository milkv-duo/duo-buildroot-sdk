
#include "Crc32.h"


// Implementation of 32-bit CRC (cyclic redundancy check) class:
//     Polynomial:                 04C11DB7
//     Initial CRC register value: FFFFFFFF
//     Reflected input and output: No
//     Inverted final output:      No
//     CRC of string "123456789":  0376E6E7

__int32 m_crc = 0xFFFFFFFF;

void CRC32_Initialize()
{
	m_crc = 0xFFFFFFFF;
}

void Crc32_Compute_Multi(const void* buffer, unsigned int count)
{
	const unsigned char* ptr = (const unsigned char *) buffer;
	while (count--) {
		Crc32_Compute_Single(*ptr++);
	}
}

void Crc32_Compute_Single(unsigned char value)
{
	int i;
	
	m_crc ^= ((unsigned __int32)value << 24);
	for (i = 0; i < 8; i++) {
		if (m_crc & 0x80000000) {
			m_crc = (m_crc << 1) ^ 0x04C11DB7;
		}
		else {
			m_crc <<= 1;
		}
	}
}

__int32 Crc32_GetResult()
{
	return m_crc;
}

__int32 Crc_Calc(const void* buffer, unsigned int count)
{
	CRC32_Initialize();
	Crc32_Compute_Multi(buffer, count);
	return Crc32_GetResult();
}





