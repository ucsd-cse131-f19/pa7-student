#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "print.h"
#include "gc.h"

// STUDENT: Change this to 1 in order to print
#define DEBUG 0

#define DEBUG_PRINT(...) \
            do { if (DEBUG) fprintf(stdout, __VA_ARGS__); } while (0)


int64_t* gc(int64_t* stack_bottom,
			int64_t* first_frame,
			int64_t* stack_top,
			int64_t* heap_start,
			int64_t* heap_end,
			int64_t* alloc_ptr) {
  DEBUG_PRINT("starting GC...\n");
  DEBUG_PRINT("\tstack top    = 0x%p\n\tstack_bottom = 0x%p\n\tfirst_frame  = 0x%p\n\theap start   = 0x%p\n\theap_end     = 0x%p\n",
			  stack_top,
			  stack_bottom,
			  first_frame,
			  heap_start,
			  heap_end);
  
  // STUDENT: You will change this code (and edit anything you like about debug
  // prints, etc), to collect garbage and finally return the new value for R15
  // to use to start allocation from.

  DEBUG_PRINT("(gc) Not yet implemented.");
  
  return alloc_ptr;
}

