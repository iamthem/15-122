#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "lib/xalloc.h"
#include "lib/stack.h"
#include "lib/contracts.h"
#include "lib/c0v_stack.h"
#include "lib/c0vm.h"
#include "lib/c0vm_c0ffi.h"
#include "lib/c0vm_abort.h"

/* call stack frames */
typedef struct frame_info frame;
struct frame_info {
  c0v_stack_t S; /* Operand stack of C0 values */
  ubyte *P;      /* Function body */
  size_t pc;     /* Program counter */
  c0_value *V;   /* The local variables */
};

void push_int(c0v_stack_t S, int i) {
    c0v_push(S, int2val(i));
    return;
}
void push_ptr(c0v_stack_t S, void* a){
    c0v_push(S, ptr2val(a));
    return;
}
void* pop_ptr(c0v_stack_t S) {
    return val2ptr(c0v_pop(S));
}

int pop_int(c0v_stack_t S){
    return val2int(c0v_pop(S));
}


int execute(struct bc0_file *bc0) {
  REQUIRES(bc0 != NULL);

  /* Variables */
  c0v_stack_t S; /* Operand stack of C0 values */
  ubyte *P;      /* The array of bytes that make up the current function */
  size_t pc;     /* Your current location within the current byte array P */
  c0_value *V;   /* The local variables (you won't need this till Task 2) */

  /* The call stack, a generic stack that should contain pointers to frames */
  /* You won't need this until you implement functions. */
  gstack_t callStack;
  callStack = stack_new();

  //Initialization of S, P, and pc
  //Don't forget to eventually free S
  S = c0v_stack_new();
  P = bc0->function_pool[0].code;
  pc = 0;
  V = xmalloc(sizeof(c0_value) * bc0->function_pool[0].num_vars);


  while (true) {

#ifdef DEBUG
        printf("Opcode %x -- Stack size: %zu -- PC: %zu\n",
                P[pc], c0v_stack_size(S), pc);
    /* You can add extra debugging information here */
#endif

    switch (P[pc]) {

    /* Additional stack operation: */

    case POP: {
      pc++;
      c0v_pop(S);
      break;
    }

    case DUP: {
      pc++;
      c0_value v = c0v_pop(S);
      c0v_push(S,v);
      c0v_push(S,v);
      break;
    }

    case SWAP: {
      pc++;
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      c0v_push(S,v1);
      c0v_push(S,v2);
      break;
    }


    /* Returning from a function.
     * This currently has a memory leak! You will need to make a slight
     * change for the initial tasks to avoid leaking memory.  You will
     * need to be revise it further when you write INVOKESTATIC. */

    case RETURN: {

      c0_value retval = c0v_pop(S);
      assert(c0v_stack_empty(S));
      c0v_stack_free(S);
      free(V);

      if (stack_size(callStack) == 0) {
          stack_free(callStack, NULL);
          return val2int(retval);
        }
      else {
          frame *F = (frame*) pop(callStack);
          V = F->V;
          S = F->S;
          P = F->P;
          pc = F->pc;
          free(F);
          c0v_push(S, retval);
          break;
      }
    }


    /* Arithmetic and Logical operations */

    case IADD:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        push_int(S, v2+v1);
        break;
    }

    case ISUB:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        push_int(S, v2-v1);
        break;
    }

    case IMUL:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        push_int(S, v2*v1);
        break;
    }

    case IDIV:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        if (v1 == 0) c0_arith_error("Division by 0\n");
        else if (v2 == INT_MIN && v1 == -1){
            c0_arith_error("Division by of int_min by -1\n");
        }
        else {
            push_int(S, v2/v1);
        }
        break;
    }

    case IREM:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        if (v1 == 0) c0_arith_error("Division by 0\n");
        else if (v2 == INT_MIN && v1 == -1){
            c0_arith_error("Division by of int_min by -1\n");
        }
        else push_int(S, v2%v1);
        break;
    }

    case IAND:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        push_int(S, v2&v1);
        break;
    }

    case IOR:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        push_int(S, v2|v1);
        break;
    }

    case IXOR:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        push_int(S, v2^v1);
        break;
    }

    case ISHL:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        if (v1 < 0) c0_arith_error("Can't left shift by negative value\n");
        else if (v1 >= 32) {
            c0_arith_error("Can't left shift by value greater than 32\n");
        }
        else push_int(S, v2 << v1);
        break;
    }

    case ISHR:
    {
        pc++;
        int v1 = pop_int(S);
        int v2 = pop_int(S);
        if (v1 < 0) c0_arith_error("Can't right shift by negative value\n");
        else if (v1 >= 32) {
            c0_arith_error("Can't right shift by value greater than 32\n");
        }
        else push_int(S, v2 >> v1);
        break;
    }


    /* Pushing constants */

    case BIPUSH:
    {
        pc++;
        int b = (int)(int8_t)P[pc];
        push_int(S, b);
        pc++;
        break;
    }

    case ILDC:
    {
        pc++;
        uint16_t index = ((uint16_t)P[pc]) << 8;
        pc++;
        index |= (uint16_t)P[pc];
        int x = bc0->int_pool[index];
        push_int(S, x);
        pc++;
        break;
    }

    case ALDC: {
        pc++;
        uint16_t index = ((uint16_t)P[pc]) << 8;
        pc++;
        index |= (uint16_t)P[pc];
        char* a = &bc0->string_pool[index];
        void *p = (void*) a;
        c0v_push(S, ptr2val(p));
        pc++;
        break;

    }

    case ACONST_NULL:
    {
        pc++;
        push_ptr(S, (void*) 0);
        break;
    }


    /* Operations on local variables */

    case VLOAD:
    {
        pc++;
        c0v_push(S, V[P[pc]]);
        pc++;
        break;
    }

    case VSTORE: {
        pc++;
        V[P[pc]] = c0v_pop(S);
        pc++;
        break;
    }


    /* Control flow operations */

    case NOP:
    {
        pc++;
        break;
    }

    case IF_CMPEQ:
    {
        pc++;
        if (val_equal(c0v_pop(S), c0v_pop(S))){
        uint16_t tmp = (uint16_t)P[pc] << 8;
        pc++;
        tmp |= (uint16_t)P[pc];
        int16_t index =  (int16_t)tmp;
        if (index != 0) pc += index - 2;
        else pc++;
        break;
        }
        else pc += 2;
        break;
    }

    case IF_CMPNE:
    {
        pc++;
        if (!val_equal(c0v_pop(S), c0v_pop(S))){
        uint16_t tmp = (uint16_t)P[pc] << 8;
        pc++;
        tmp |= (uint16_t)P[pc];
        int16_t index =  (int16_t)tmp;
        if (index != 0) pc += index - 2;
        else pc++;
        break;
        }
        else pc += 2;
        break;
    }

    case IF_ICMPLT:
    {
        pc++;
        if (c0v_pop(S).payload.i > c0v_pop(S).payload.i){
        uint16_t tmp = (uint16_t)P[pc] << 8;
        pc++;
        tmp |= (uint16_t)P[pc];
        int16_t index =  (int16_t)tmp;
        if (index != 0) pc += index - 2;
        else pc++;
        break;
        }
        else pc += 2;
        break;
    }

    case IF_ICMPGE:
    {
        pc++;
        if (c0v_pop(S).payload.i <= c0v_pop(S).payload.i){
        uint16_t tmp = (uint16_t)P[pc] << 8;
        pc++;
        tmp |= (uint16_t)P[pc];
        int16_t index =  (int16_t)tmp;
        if (index != 0) pc += index - 2;
        else pc++;
        break;
        }
        else pc += 2;
        break;
    }

    case IF_ICMPGT:
    {
        pc++;
        if (c0v_pop(S).payload.i < c0v_pop(S).payload.i){
        uint16_t tmp = (uint16_t)P[pc] << 8;
        pc++;
        tmp |= (uint16_t)P[pc];
        int16_t index =  (int16_t)tmp;
        if (index != 0) pc += index - 2;
        else pc++;
        break;
        }
        else pc += 2;
        break;
    }

    case IF_ICMPLE:
    {
        pc++;
        if (c0v_pop(S).payload.i >= c0v_pop(S).payload.i){
            uint16_t tmp = (uint16_t)P[pc] << 8;
            pc++;
            tmp |= (uint16_t)P[pc];
            int16_t index =  (int16_t)tmp;
            if (index != 0) pc += index - 2;
            else pc++;
            break;
        }
        else pc += 2;
        break;
    }


    case GOTO:
    {
        pc++;
        uint16_t tmp = (uint16_t)P[pc] << 8;
        pc++;
        tmp |= (uint16_t)P[pc];
        int16_t index =  (int16_t)tmp;
        if (index != 0) pc += index - 2;
        else pc++;
        break;
    }

    case ATHROW:
    {
        pc++;
        c0_user_error((char*)pop_ptr(S)); break;
    }

    case ASSERT:
    {
        pc++;
        char* msg = (char*)pop_ptr(S);
        if (pop_int(S) == 0) c0_assertion_failure(msg);
        break;
    }

    /* Function call operations: */

    case INVOKESTATIC:
    {

        //Build a stack frame containing current values
        frame* F0 = xmalloc(sizeof(frame));
        F0->P = P;
        F0->pc = pc+3;
        F0->V = V;
        F0->S = S;
        push(callStack, (void*) F0);

        //Get index
        pc++;
        uint16_t index = ((uint16_t)P[pc]) << 8;
        pc++;
        index |= (uint16_t)P[pc];

        //Allocate a new locals array of size fi.num_vars

        struct function_info f = bc0->function_pool[index];
        int len = (int) f.num_vars;
        int numargs = (int) f.num_args;
        V = xmalloc(sizeof(c0_value) * len);
        for (int i = numargs-1; i >= 0; i--) {
            V[i] = c0v_pop(S);
        }


        //Create new empty operand stack
        S = c0v_stack_new();


        //Load the new code into the program array.
        P = f.code;

        //Set the program counter to 0.
        pc = 0;

        break;
    }

    case INVOKENATIVE:
    {
        pc++;
        uint16_t index = ((uint16_t)P[pc]) << 8;
        pc++;
        index |= (uint16_t)P[pc];


        //Make array of c0_values
        int len = (int) bc0->native_pool[index].num_args;
        c0_value A [len];
        for (int i = len-1; i >= 0; i--) {
            A[i] = c0v_pop(S);
        }

        //Get function index
        uint16_t function_index = bc0->native_pool[index].function_table_index;

        //Get result from function and push it on operand stack
        c0_value result = (*native_function_table[function_index])(A);
        c0v_push(S, result);
        pc++;
        printf("Opcode %x -- Stack size: %zu -- PC: %zu\n",
                P[pc], c0v_stack_size(S), pc);
        break;
    }


    /* Memory allocation operations: */

    case NEW:
    {
        pc++;
        int b = (int)(int8_t)P[pc];
        void *p = xmalloc(b);
        push_ptr(S, p);
        pc++;
        break;
    }

    case NEWARRAY:
    {
        pc++;
        int elt_size = (int)(int8_t)P[pc];
        int n = pop_int(S);
        c0_array *A = xmalloc(sizeof(c0_array));
        A->count = n;
        A->elt_size = elt_size;
        A->elems = xcalloc(n, elt_size);
        push_ptr(S, (void*)A);
        pc++;
        break;
    }

    case ARRAYLENGTH:
    {
        c0_array *A = pop_ptr(S);
        if (A == NULL) c0_memory_error("array is NULL\n");
        push_int(S, A->count);
        pc++;
        break;
    }

    /* Memory access operations: */

    case AADDF:
    {
        pc++;
        ubyte f = (ubyte)P[pc];
        char *a = pop_ptr(S);
        if (a == NULL) c0_memory_error("pointer is NULL\n");
        void *p = a+f;
        push_ptr(S, p);
        pc++;
        break;
    }

    case AADDS:
    {
        int n = pop_int(S);
        c0_array *A = pop_ptr(S);
        if (A == NULL) c0_memory_error("pointer is NULL\n");
        if (0 <= n && n < A->count) {
            char *a = A->elems;
            void *p = a+(n * A->elt_size);
            push_ptr(S, p);
            pc++;
            break;
        }
        else c0_memory_error("index out of bounds\n");
    }
    case IMLOAD:
    {
        int *p = pop_ptr(S);
        if (p == NULL) c0_memory_error("pointer is NULL\n");
        int x = *p;
        push_int(S, x);
        pc++;
        break;
    }

    case IMSTORE:
    {
        int x = pop_int(S);
        int *p = (int*)pop_ptr(S);
        if (p == NULL) c0_memory_error("pointer is NULL\n");
        *p = x;
        pc++;
        break;
    }

    case AMLOAD:
    {
        void **a = pop_ptr(S);
        if (a == NULL) c0_memory_error("pointer is NULL\n");
        void *b = *a;
        push_ptr(S, b);
        pc++;
        break;
    }

    case AMSTORE:
    {
        void *b = pop_ptr(S);
        void **a = pop_ptr(S);
        if (a == NULL) c0_memory_error("pointer is NULL\n");
        *a = b;
        pc++;
        break;
    }

    case CMLOAD:
    {
        char *p = pop_ptr(S);
        if (p == NULL) c0_memory_error("pointer is NULL\n");
        int x = (int32_t)(*p);
        push_int(S, x);
        pc++;
        break;
    }

    case CMSTORE:
    {
        pc++;
        int x = pop_int(S);
        char* a = pop_ptr(S);
        if (a == NULL) c0_memory_error("pointer is NULL\n");
        *a = x & 0x7f;
        break;
    }

    default:
      abort();
    }
  }

  /* cannot get here from infinite loop */
  assert(false);
}
