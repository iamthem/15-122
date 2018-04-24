#include <stdlib.h>
#include <stdbool.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"



typedef struct list_node list;
struct list_node {
    void* data;
    list* next;
};

typedef struct queue_header queue;
struct queue_header {
    list* front;
    list* back;
    int size;
};

bool is_inclusive_segment(list* start, list* end, int i) {
    if (i == 0 && start == NULL) return true;
    else if (i == 0 && start != NULL) return false;
    else if (start == NULL) return false;
    return is_inclusive_segment(start->next, end, i-1);
}

bool is_queue(queue* Q) {
    return Q != NULL && is_inclusive_segment(Q->front, Q->back, Q->size);
}

queue_t queue_new()
//@ensures is_queue(\result);
{
    queue* Q = xmalloc(sizeof(queue));
    Q->size = 0;
    Q->front = NULL;
    ENSURES(Q != NULL);
    ENSURES(is_queue(Q));
    return Q;
}

size_t queue_size(queue_t Q)
//@requires is_queue(Q);
//@ensures \result >= 0;
{
    REQUIRES(Q != NULL);
    REQUIRES(is_queue(Q));
    ENSURES(Q->size >= 0);
    return Q->size;
}

void enq(queue_t Q, void* x)
//@requires is_queue(Q);
//@ensures is_queue(Q);
{
    REQUIRES(Q != NULL);
    REQUIRES(is_queue(Q));
    list* l = xmalloc(sizeof(list));
    l->next = NULL;
    l->data = x;
    if (queue_size(Q) == 0) {
        Q->front = l;
        Q->back = l;
    } else {
        Q->back->next = l;
        Q->back = l;
    }
    Q->size++;
    ENSURES(is_queue(Q));
}

void* deq(queue_t Q)
//@requires is_queue(Q) && queue_size(Q) > 0;
//@ensures is_queue(Q);
{
    REQUIRES(Q != NULL);
    REQUIRES(is_queue(Q));
    REQUIRES(queue_size(Q) > 0);
    void* s = Q->front->data;
    if (queue_size(Q) != 1) {
        list* l = Q->front;
        Q->front = Q->front->next;
        free(l);
        Q->size--;
    } else {
        list* tmp = Q->front;
        free(tmp);
        Q->front = NULL;
        Q->size--;
    }
    ENSURES(is_queue(Q));
    return s;
}

void *queue_peek(queue_t Q, size_t i)
//@requires is_queue(Q) && 0 <= i && i < queue_size(Q);
{
    REQUIRES(Q != NULL);
    REQUIRES(is_queue(Q));
    REQUIRES(i < queue_size(Q));
    list* l = Q->front;
    size_t j = 0;
    while (j < i) {
        l = l->next;
        j++;
    }
    ENSURES(is_queue(Q));
    return l->data;
}

list* helper_next(list* l, int i)
//@requires l != NULL;
{
    REQUIRES(l != NULL);
    if (i == 0) return l;
    return helper_next(l->next, i-1);
}

void queue_reverse(queue_t Q)
//@requires is_queue(Q);
{
    REQUIRES(Q != NULL);
    REQUIRES(is_queue(Q));
    if (Q->size < 2) return;
    list* curr = Q->back;
    list* tmp = curr;
    for (int i = Q->size-2; i > -1; i--) //O(n)
    //@loop_invariant -1 <= i && i <= Q->size - 2;
    //@loop_invariant curr != NULL;
    {
        curr->next = helper_next(Q->front, i); //O(i)
        curr = curr->next;
    }
    curr->next = NULL;
    Q->front = tmp;
    Q->back = curr;
    ENSURES(is_queue(Q));
}

bool queue_all(queue_t Q, check_property_fn* P)
//@requires is_queue(Q) && P != NULL;
{
    REQUIRES(Q != NULL);
    REQUIRES(is_queue(Q));
    REQUIRES(P != NULL);
    if (Q->size == 0) return true;
    for (int i = 0; i < Q->size; i++)
    {
        if (!(*P)(queue_peek(Q, i))) return false;
    }
    ENSURES(is_queue(Q));
    return true;
}

void* queue_iterate(queue_t Q, void* base, iterate_fn* F)
//@requires is_queue(Q) && F != NULL;
{
    REQUIRES(Q != NULL);
    REQUIRES(is_queue(Q));
    REQUIRES(F != NULL);
    if (Q->size == 0) return base;
    list* curr = Q->front;
    void* tmp = base;
    while (curr != Q->back)
    //@loop_invariant curr != NULL;
    {
        tmp = (*F)(tmp, curr->data);
        curr = curr->next;
    }
    tmp = (*F)(tmp, curr->data);
    ENSURES(is_queue(Q));
    return tmp;
}

void queue_free(queue_t Q, free_fn* F)
//@requires Q != NULL; @*/ ;
{
    REQUIRES(Q != NULL);
    REQUIRES(is_queue(Q));
    if (queue_size(Q) > 0) {
        if (F == NULL) {
            list* l = Q->front;
            while(l != Q->back) {
                list* tmp = l;
                l = l->next;
                free(tmp);
            }
            free(l);
        }else {
            list* l = Q->front;
            while(l != Q->back) {
                list* tmp = l;
                l = l->next;
                (*F)(tmp);
            }
            free(l);
        }
    }
    free(Q);
}
