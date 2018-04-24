#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib/xalloc.h"
#include "lib/contracts.h"
#include "lib/bitvector.h"

#if (BITVECTOR_LIMIT <= 8)
#define TYPE uint8_t

#elif (BITVECTOR_LIMIT <= 16)
#define TYPE uint16_t

#elif (BITVECTOR_LIMIT <= 32)
#define TYPE uint32_t

#else
#define TYPE uint64_t

#endif

bitvector bitvector_new() {
    TYPE result = 0;
    return result;
}

bool bitvector_get(bitvector B, uint8_t i)
  /*@requires 0 <= i < BITVECTOR_LIMIT; @*/
{
    REQUIRES(i < BITVECTOR_LIMIT);
    TYPE ith_bit = (TYPE) i;
    return ((((TYPE)1 << ith_bit) & B) != 0);
}

bitvector bitvector_flip(bitvector B, uint8_t i)
  /*@requires 0 <= i < BITVECTOR_LIMIT; @*/
{
    REQUIRES(i < BITVECTOR_LIMIT);
    TYPE ith_bit = (TYPE) i;
    return (B ^ (TYPE)1 << ith_bit);
}

bool bitvector_equal(bitvector B1, bitvector B2)
{
    TYPE a = (TYPE) B1;
    TYPE b = (TYPE) B2;
    return a == b;
}

