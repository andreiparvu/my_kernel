#include "asm.h"
#include <stddef.h>

#define UART0 ((volatile unsigned int*)0x101f1000)

#define UARTFR 0x06
#define UARTFR_TXFF 0x20

#define STACK_SIZE 256
#define TASK_LIMIT 10

void bwputs(char *s) {
  while(*s) {
    while(*(UART0 + UARTFR) & UARTFR_TXFF);
    *UART0 = *s;
    s++;
  }
}

void *memcpy(void *dest, const void *src, size_t n) {
  char *d = dest;
  const char *s = src;
  size_t i;
  for(i = 0; i < n; i++) {
    d[i] = s[i];
  }
  return d;
}

unsigned int *init_task(unsigned int *stack, void (*start)(void)) {
  stack += STACK_SIZE - 16; /* End of stack, minus what we're about to push */
  stack[0] = 0x10; /* User mode, interrupts on */
  stack[1] = (unsigned int)start;

  return stack;
}

void first(void) {
  bwputs("In user mode\n");

  while(1) syscall();
}

void task(void) {
  bwputs("In other task\n");
  while(1) syscall();
}

int main(void) {
  unsigned int stacks[TASK_LIMIT][STACK_SIZE];
  unsigned int *tasks[TASK_LIMIT];

  size_t task_count = 0;
  size_t current_task = 0;

  tasks[0] = init_task(stacks[0], &first);
  tasks[1] = init_task(stacks[1], &task);
  task_count = 2;

  while (1) {
    tasks[current_task] = activate(tasks[current_task]);

    switch(tasks[current_task][2+7]) {
      case 0x1:
        if (task_count == TASK_LIMIT) {
          tasks[current_task][2+0] = -1;
        } else {
          size_t used = stacks[current_task] + STACK_SIZE - tasks[current_task];
          tasks[task_count] = stacks[task_count] + STACK_SIZE - used;
          memcpy(tasks[task_count], tasks[current_task],
                 used*sizeof(*tasks[current_task]));
          tasks[current_task][2+0] = task_count;
          tasks[task_count][2+0] = 0;
          task_count++;
        }
        break;
    }

    current_task++;
    if(current_task >= task_count) current_task = 0;
  }

  while(1); /* We can't exit, there's nowhere to go */
}
