#include "EndianConvert.h"

static int checkEndian()
{
  union {
    uint32_t i;
    unsigned char s[4];
  }c;

  c.i = 0x12345678;
  return (0x12 == c.s[0]);
}

uint64_t __attribute__((weak)) htobe64(uint64_t v)
{
  return checkEndian() ? v : BigLittleSwap64(v);
}

uint64_t __attribute__((weak)) be64toh(uint64_t v)
{
  return checkEndian() ? v : BigLittleSwap64(v);
}