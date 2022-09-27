#ifndef MD5_H__
#define MD5_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../Logger.h"

void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

int hashEqual(const uint8_t *expect, const uint8_t *actual);

#endif