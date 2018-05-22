#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// STUDENT: Change this to 1 in order to print
#define DEBUG 0

#define DEBUG_PRINT(...) \
            do { if (DEBUG) fprintf(stdout, __VA_ARGS__); } while (0)


void print_mem(int* heap, int size) {
  for(int i = 0; i < size; i += 1) {
    DEBUG_PRINT("  %d/%p: %p (%d)\n", i, (heap + i), (int*)(*(heap + i)), *(heap + i));
  }
}

int* mark(int* stack_top, int* first_frame, int* stack_bottom, int* max_addr) {
  return NULL;
}

void forward(int* stack_top, int* first_frame, int* stack_bottom, int* heap_start, int* max_address) {
  return;
}

int* compact(int* heap_start, int* max_address) {
  return NULL;
}

int* gc(int* stack_bottom, int* first_frame, int* stack_top, int* heap_start, int* heap_end, int* alloc_ptr) {

  DEBUG_PRINT("");
  print_mem(stack_top, 10);
  DEBUG_PRINT("\n\n");
  print_mem(heap_start, 10);

  // STUDENT: You will change this code (and edit anything you like about debug
  // prints, etc), to use mark, forward, and compact to collect garbage and
  // finally return the new value for EBX to use to start allocation from.

  return alloc_ptr;
}

