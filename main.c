#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "gc.h"
#define TRUE 0xFFFFFFFE
#define FALSE 0x7FFFFFFE
#define TUPLE_MASK 0x3
#define TUPLE_NULL 0x0

#define NUM_MIN (- (1 << 30))
#define NUM_MAX ((1 << 30) - 1)

extern int* try_gc(int* alloc_ptr, int amount_needed, int* first_frame, int* stack_top) asm("try_gc");
extern int* HEAP_END asm("HEAP_END");
extern int* STACK_BOTTOM asm("STACK_BOTTOM");

extern int our_code_starts_here(int input_val, int * HEAP) asm("our_code_starts_here");
void error(int val) asm("error");
int print(int val) asm("print");
int deep_equals(int val1, int val2) asm("deep_equals");
int tup_equals(int * tup1, int * tup2);
void tup_print(int val);

size_t HEAP_SIZE;  // This is set in main by the first command-line argument
int* STACK_BOTTOM; // This is set in generated code
int* HEAP;         // This is set in main
int* HEAP_END;     // This is set in main

int DEFAULT_HEAP_SIZE = 100000;

int deep_equals(int val1, int val2) {
    if (!(val1 & TUPLE_MASK) && !(val2 & TUPLE_MASK)) {
        return tup_equals((int *)val1, (int *) val2);
    }

    return val1 == val2 ? TRUE : FALSE;
}

int tup_equals(int * tup1, int * tup2) {
    int* val1 = (int*) tup1;
    int* val2 = (int*) tup2;

    if (val1 == TUPLE_NULL && val2 == TUPLE_NULL) return TRUE;

    int count1 = *(val1 + 1);
    int count2 = *(val2 + 1);

    //return false if lengths are different 
    if(count1 != count2) return FALSE; 

    //loop through all elements in tuple
    for(int i = 2; i < count1 + 2; i += 1) {
      int elem1 = *(val1 + i); 
      int elem2 = *(val2 + i); 

      if(!(elem1 & TUPLE_MASK) && !(elem2 & TUPLE_MASK)) { 
        return tup_equals((int *) elem1, (int *) elem2); 
      } else {
        if (elem1 != elem2) { 
          return FALSE; 
        }
      }
    }
    return TRUE; 

}

void print_simple(int val) {
  if (val == TRUE) {
    printf("true");
  } else if (val == FALSE) {
    printf("false");
  } else if ((val & 1) == 1) {
    printf("%d", val>>1);
  } else if (val == TUPLE_NULL) {
    printf("()");
  } else if(!(val & TUPLE_MASK)) {
    tup_print(val);
  } else {
    printf("Unknown value: %#010x", val);
  } 
}

void tup_print(int val){
  printf("(");
  int* valp = (int*) val;
  int count = *(valp + 1);
  for(int i = 2; i < count + 2; i += 1) {
    print_simple(*(valp + i));
    if(i < count + 1) {
      printf(",");
    }
  }
  printf(")"); 
}

int print(int val) {
  print_simple(val);
  printf("\n");
  return val;
}

void error(int error_code) {
  if(error_code == 1)
    fprintf(stderr, "Error: expected a number\n");
  else if (error_code == 2)
    fprintf(stderr, "Error: expected a boolean\n");
  else if (error_code == 3)
    fprintf(stderr, "Error: overflow\n");
  else if (error_code == 4)
    fprintf(stderr, "Error: input must be a boolean or a number\n");
  else if (error_code == 5)
    fprintf(stderr, "Error: input is not a representable number\n");
  else if (error_code == 6) 
    fprintf(stderr, "Error: index too small\n");
  else if (error_code == 7) 
    fprintf(stderr, "Error: index too large\n");
  else if (error_code == 8)
    fprintf(stderr, "Error: expected a tuple\n");
  exit(123456);
}


/*
  STUDENT: You do not need to change this main function, but it's worth reading!

  It does two things that are interesting relative to Diamondback:

  1. It uses the first command-line argument as the initial size of the heap
    (in words)
  2. It initializes the heap, including global variables that are shared with
    the try_gc function

  Other than that, it's just an implementation of what you had to do in PA6,
  but shifted by one argument (since the first argument now means heap size)
*/
int main(int argc, char** argv) {
  int input_val;
  int heap_size = DEFAULT_HEAP_SIZE;
  if(argc > 1) { heap_size = atoi(argv[1]); }

  // Initialize globals that are shared outside this function
  HEAP = calloc(heap_size, sizeof (int));
  HEAP_SIZE = heap_size;
  HEAP_END = HEAP + heap_size;

  char * endptr;
  extern int errno;

  int* goodHeap = HEAP;

  input_val = FALSE;
  if ( argc > 3 )
  {
    input_val = (int) goodHeap;
    *goodHeap = 0;
    *(goodHeap + 1) = argc - 2;
    goodHeap += 2;

    for ( int index = 2; index < argc; index++ )
    {
      if (!strcmp("true", argv[index]))
        *goodHeap = TRUE;
      else if (!strcmp("false", argv[index]))
        *goodHeap = FALSE;
      else
      {
        endptr = (char*) &argv[index];
        errno = 0;
        long r = strtol(argv[index], &endptr, 10);
        if (*endptr != '\0')
          error(4);
        else if ( errno || r < NUM_MIN || r > NUM_MAX)
          error(5);

        *goodHeap = r << 1 | 1;
      }

      goodHeap++;
    }
  }
  else if (argc > 2) {
    if (!strcmp("true", argv[2])) {
      input_val = TRUE;
    } else if (!strcmp("false", argv[2])) {
      input_val = FALSE;
    } else {
      endptr = (char*) &argv[1];
      errno = 0;
      long r = strtol(argv[2], &endptr, 10);
      if (*endptr != '\0') {
        error(4);
      }
      else if ( errno || r < NUM_MIN || r > NUM_MAX) {
        error(5);
      }
      input_val = r << 1 | 1;
    }
  }
  int result;
  result = our_code_starts_here(input_val, goodHeap);
  print(result);
  free( HEAP );
  return 0;
}


/*
  Try to clean up space in memory by calling gc.

  Arguments:

    - alloc_ptr: The current value of EBX (where the next value would be
      allocated without GC)
    - bytes_needed: The number of _bytes_ that the runtime is trying to allocate
    - first_frame: The current value of EBP
    - stack_top: The current value of ESP

  Returns:

    The new value for EBX, for the runtime to start using as the allocation
    point.  Must be set to a location that provides enough room to fit
    bytes_needed more bytes in the given heap space if possible.

*/
int* try_gc(int* alloc_ptr, int bytes_needed, int* first_frame, int* stack_top) {
  // If we are at the very beginning of the heap and try to allocate something
  // too large, that's a special error – there's no way this value can fit!
  if(HEAP == alloc_ptr) {
    fprintf(stderr, "Allocation of %d words too large for %d-word heap\n", bytes_needed / 4, (int)HEAP_SIZE);
    free(HEAP);
    exit(10);
  }
  // STUDENT: this is the main entry-point to your code!
  int* new_ebx = gc(STACK_BOTTOM, first_frame, stack_top, HEAP, HEAP_END, alloc_ptr);
  if((new_ebx + (bytes_needed / 4)) > HEAP_END) {
    fprintf(stderr, "Out of memory: needed %d words, but only %d remain after collection", bytes_needed / 4, (HEAP_END - new_ebx));
    free(HEAP);
    exit(9);
  }
  else {
    return new_ebx;
  }
}

