// Interface definition for 32-bit CRC (cyclic redundancy check) class:
//     Polynomial:                 04C11DB7
//     Initial CRC register value: FFFFFFFF
//     Reflected input and output: No
//     Inverted final output:      No
//     CRC of string "123456789":  0376E6E7

void CRC32_Initialize();

void Crc32_Compute_Multi(const void* buffer, unsigned int count);

void Crc32_Compute_Single(unsigned char value);

__int32 Crc32_GetResult();

__int32 Crc_Calc(const void* buffer, unsigned int count);







