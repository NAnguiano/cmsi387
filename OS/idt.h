#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

struct cpu_state {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
} __attribute__((packed));

struct stack_state {
    unsigned int error_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} __attribute__((packed));

void interrupt_handler(struct cpu_state cpu, struct stack_state stack, unsigned int interrupt) {
    // DO STUFF HERE?!
    // I tried with having it return something too. Didn't work.
    // TELL ME WHERE I FAILED *_*
}

void load_idt(void* address);

#endif /* INCLUDE_IO_H */