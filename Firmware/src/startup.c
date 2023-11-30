#include <stdint.h>
#include <time.h>

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

// Adapted from https://stackoverflow.com/questions/58947716/how-to-interact-with-risc-v-csrs-by-using-gcc-c-code
__attribute__((always_inline)) inline uint32_t csr_mstatus_read(void){
    uint32_t result;
    asm volatile ("csrr %0, mstatus" : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void csr_mstatus_write(uint32_t val){
    asm volatile ("csrw mstatus, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_write_mie(uint32_t val){
    asm volatile ("csrw mie, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_enable_interrupts(void){
    asm volatile ("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void){
    asm volatile ("csrci mstatus, 0x8");
}

#define INTERRUPT_PENDING_REGISTER (*((volatile uint32_t *)0x40000004))
#define MTIME_LOW       (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH      (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW    (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH   (*((volatile uint32_t *)0x40000014))
#define CONTROLLER      (*((volatile uint32_t *)0x40000018))



enum States{
    RUNNING,
    BLOCKED,
    READY,
    TERMINATED,
    WAITING
};

typedef struct TCB{
    uint32_t thread_id;
    enum States thread_state;
} TCB;

struct TCB* thread_control_sys;

typedef void (*TThreadEntry)(void *);

void init(void){
    uint8_t *Source = _erodata;
    uint8_t *Base = _data < _sdata ? _data : _sdata;
    uint8_t *End = _edata > _esdata ? _edata : _esdata;

    while(Base < End){
        *Base++ = *Source++;
    }
    Base = _bss;
    End = _ebss;
    while(Base < End){
        *Base++ = 0;
    }

    csr_write_mie(0x888);       // Enable all interrupt soruces
    csr_enable_interrupts();    // Global interrupt enable
    MTIMECMP_LOW = 1;
    MTIMECMP_HIGH = 0;

    thread_control_sys = (struct TCB*)malloc(10 * sizeof(struct TCB));
}

extern volatile int global;
extern volatile uint32_t controller_status;
extern volatile int reset;

void c_interrupt_handler(uint32_t mcause){

    if(mcause == 2147483655) {
        uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH)<<32) | MTIMECMP_LOW;
        NewCompare += 100;
        MTIMECMP_HIGH = NewCompare>>32;
        MTIMECMP_LOW = NewCompare;  
        global++;
        controller_status = CONTROLLER;
    }
    else if (mcause == 2147483659){
        if(INTERRUPT_PENDING_REGISTER & 0x2) {
            global++;
            controller_status = CONTROLLER;
            INTERRUPT_PENDING_REGISTER = (INTERRUPT_PENDING_REGISTER & 0xFFFB);
        }
    }

    if(INTERRUPT_PENDING_REGISTER & 0x4) {
        if (reset == 1) {
            reset--;
        }
        else {
            reset++;
        }
        INTERRUPT_PENDING_REGISTER = (INTERRUPT_PENDING_REGISTER & 0xFFFD);
    }   
}

typedef void (*TThreadEntry)(void *);
typedef uint32_t *TThreadContext;

TThreadContext InitThread(uint32_t *stacktop, TThreadEntry entry, void *param);
void SwitchThread(TThreadContext *oldcontext, TThreadContext newcontext);


uint32_t c_system_call(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t call){
    if(1 == call){
        return global;
    }
    else if(2 == call){
        return controller_status;
    }
    else if(3 == call){
        return reset;
    }
    else if(4 == call){
        return 0;  // generate event
    }


    else if(16 == call){
        return 0; // kill;
    }
    else if(17 == call){
        // wait(arg0);
        return 0;
    }
    else if(18 == call){
        return 0; // get_ppid;
    }


    else if(20 == call){
        return create_TCB(arg0, arg1);
    }
    else if(21 == call){
        SwitchThread(arg0, arg1);
        return 0;
    }

    return -1;
}

uint32_t create_TCB(TThreadEntry entry, void *param){
    uint32_t OtherThreadStack[128];

    TCB *new_TCB = (TCB*)malloc(sizeof(TCB));
    new_TCB->thread_id = InitThread(OtherThreadStack + 128, entry, param);
    new_TCB->thread_state = READY;

    return new_TCB->thread_id;
}

