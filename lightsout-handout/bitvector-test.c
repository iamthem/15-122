#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib/xalloc.h"
#include "lib/contracts.h"
#include "lib/bitvector.h"
#include "board-ht.h"
#include "lib/hdict.h"

typedef struct board_data bd;

size_t k_hash(void* k)
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


int main() {
    bitvector V = bitvector_new();
    bitvector A = bitvector_new();
    bitvector C = bitvector_new();
    bitvector D = bitvector_new();
    bitvector E = bitvector_new();
    for (int i = 0; i < 20; i += 5)
    {
        V = bitvector_flip(V, (uint8_t) i);
        A = bitvector_flip(A, (uint8_t) i);
        C = bitvector_flip(C, (uint8_t) i);
        D = bitvector_flip(D, (uint8_t) i);
    }

    A = bitvector_flip(A, (uint8_t) 3);
    C = bitvector_flip(C, (uint8_t) 5);
    D = bitvector_flip(D, (uint8_t) 10);

    printf("%zu\n", k_hash((void*)&A));
    printf("%zu\n", k_hash((void*)&C));
    printf("%zu\n", k_hash((void*)&D));
    printf("%zu\n", k_hash((void*)&E));
    printf("aids0\n");
    bd *X = xmalloc(sizeof(bd*));
    X->board = V;

    bd *Y = xmalloc(sizeof(bd*));
    Y->board = A;

    printf("aids1\n");
    bd *Z = xmalloc(sizeof(bd*));
    Z->board = E;

    hdict_t H = ht_new((size_t) 10);
    hdict_t J = ht_new((size_t) 10);

    printf("aids2\n");
    ht_insert(H, X);
    ht_insert(H, Y);
    ht_insert(H, Z);

    printf("aids3\n");
    assert(bitvector_equal(ht_lookup(H, V)->board, V));
    assert(ht_lookup(J, V) == NULL);
    assert(ht_lookup(J, D) == NULL);
    assert(ht_lookup(J, C) == NULL);
    printf("aids4\n");
    assert(ht_lookup(J, E) == NULL);
    printf("aids5\n");
    printf("aids6\n");
    assert(bitvector_equal(ht_lookup(H, A)->board, A));

    printf("aids7\n");
    hdict_free(H);
    hdict_free(J);
    return 0;
}
