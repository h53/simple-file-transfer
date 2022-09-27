#ifndef ENDIAN_CONVERT_H__
#define ENDIAN_CONVERT_H__

#include <stdint.h>

#define BigLittleSwap64(A) ((((uint64_t)(A) & 0xff00000000000000) >> 56) | \
                            (((uint64_t)(A) & 0x00ff000000000000) >> 40) | \
                            (((uint64_t)(A) & 0x0000ff0000000000) >> 24) | \
                            (((uint64_t)(A) & 0x000000ff00000000) >> 8 ) | \
                            (((uint64_t)(A) & 0x00000000ff000000) << 8 ) | \
                            (((uint64_t)(A) & 0x0000000000ff0000) << 24) | \
                            (((uint64_t)(A) & 0x000000000000ff00) << 40) | \
                            (((uint64_t)(A) & 0x00000000000000ff) << 56))

//mac doesnt have this functions below
extern uint64_t htobe64(uint64_t v);
extern uint64_t be64toh(uint64_t v);

#endif //ENDIAN_CONVERT_H__