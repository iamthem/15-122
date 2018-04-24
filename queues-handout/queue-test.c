#include <stdlib.h>
#include <stdbool.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"

int main() {

    queue_t A = queue_new();
    int* p1 = xmalloc(sizeof(int));
    int* p2 = xmalloc(sizeof(int));
    int* p3 = xmalloc(sizeof(int));
    int* p4 = xmalloc(sizeof(int));
    int* p5 = xmalloc(sizeof(int));
    int* p6 = xmalloc(sizeof(int));
    int* p7 = xmalloc(sizeof(int));
    int* p8 = xmalloc(sizeof(int));

    *p1 = 1;
    *p2 = 2;
    *p3 = 3;
    *p4 = 4;
    *p5 = 5;
    *p6 = 6;
    *p7 = 7;
    *p8 = 8;
    enq(A, (void*)p1);
    enq(A, (void*)p2);
    enq(A, (void*)p3);
    enq(A, (void*)p4);

    assert(queue_size(A) == 4);
    assert(*(int*)deq(A) == 1);
    assert(*(int*)deq(A) == 2);
    assert(*(int*)deq(A) == 3);
    assert(*(int*)deq(A) == 4);

    queue_t B = queue_new();
    enq(B, (void*)p5);
    enq(B, (void*)p6);
    enq(B, (void*)p7);
    enq(B, (void*)p8);
    assert(*(int*)queue_peek(B, 0) == 5);
    assert(*(int*)queue_peek(B, 1) == 6);
    assert(*(int*)queue_peek(B, 2) == 7);
    assert(*(int*)queue_peek(B, 3) == 8);
    queue_reverse(B);
    assert(*(int*)queue_peek(B, 0) == 8);
    assert(*(int*)queue_peek(B, 1) == 7);
    assert(*(int*)queue_peek(B, 2) == 6);
    assert(*(int*)queue_peek(B, 3) == 5);

    queue_free(A, NULL);
    queue_free(B, NULL);
    free(p1);
    free(p2);
    free(p3);
    free(p4);
    free(p5);
    free(p6);
    free(p7);
    free(p8);

    return 0;
}
