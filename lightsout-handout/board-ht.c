#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "lib/xalloc.h"
#include "lib/contracts.h"
#include "lib/bitvector.h"
#include "lib/hdict.h"
#include "board-ht.h"

// Make abreviation to make life simple
typedef struct board_data bd;

size_t key_hash(void* k)
//@requires k != NULL && \hastag(bitvector, k)
{
    bitvector B = *((bitvector*) k);
    size_t hash = 0;
    for (int i = 0; i < BITVECTOR_LIMIT; i++) {
        bool tmp = bitvector_get(B, i);
        if (tmp) hash++;
        hash = hash << 1;
    }
    return hash;

}

bool key_equal(void* k1, void* k2)
//@requires k1 != NULL && k2 != NULL
{
    bitvector B1 = *((bitvector*) k1);
    bitvector B2 = *((bitvector*) k2);
    return bitvector_equal(B1, B2);
}

void value_free(void* v)
{
    bd *free_me = (bd*) v;
    free(free_me);
    return;
}

hdict_t ht_new(size_t capacity)
  /*@requires capacity > 0; @*/
  /*@ensures \result != NULL; @*/
{
    hdict_t result = hdict_new(capacity, &key_equal, &key_hash, &value_free);
    return result;
}

bd *ht_lookup(hdict_t H, bitvector B)
  /*@requires H != NULL; @*/
{
    REQUIRES(H != NULL);
    return hdict_lookup(H, (void*) &B);
}

void ht_insert(hdict_t H, bd *DAT)
  /*@requires H != NULL; @*/
  /*@requires DAT != NULL; @*/

{
    REQUIRES(H != NULL);
    REQUIRES(DAT != NULL);
    REQUIRES((ht_lookup(H, DAT->board) == NULL));
    hdict_insert(H, (void*) &DAT->board, (void*) DAT);
    return;
}

