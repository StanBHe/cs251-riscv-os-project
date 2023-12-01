#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "./syscalls/sprites.h"

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
#define SAVE_DATA       0x70700000

char *save_data [10];

enum States{
    RUNNING,
    BLOCKED,
    READY,
    TERMINATED,
    WAITING
};

typedef uint32_t *TThreadContext;

typedef struct TCB{
    uint32_t thread_id;
    enum States thread_state;
} TCB;

struct TCB* thread_control_sys;

typedef void (*TThreadEntry)(void *);

extern volatile int global;
extern volatile uint32_t controller_status;
extern volatile int reset;

typedef void (*TThreadEntry)(void *);


TThreadContext InitThread(uint32_t *stacktop, TThreadEntry entry, void *param);
void SwitchThread(TThreadContext *oldcontext, TThreadContext newcontext);

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

uint32_t os_get_all_saves(){
    uint32_t save_address = SAVE_DATA;
    int counter = 0;
    char *empty_str = "Empty";
    while(save_address <= 0x70800000){
        if((*((volatile uint32_t *)save_address)) == 0){
           save_data[counter] = empty_str;
        }else{
           save_data[counter] = (char *) save_address;
        }
        save_address += 0x10000;
        counter++;
    }
    return save_data;
}

uint32_t os_get_save(uint32_t arg0){
    uint32_t save_address = SAVE_DATA;
    while(save_address <= 0x70800000){
        int all_same = 1;
        if((*((volatile uint32_t *)save_address)) != 0){
            uint32_t temp_save = save_address;
            uint32_t temp_arg0 = arg0;
            while(all_same == 1 && (*((volatile char *)temp_save)) != '\0' && (*((char *)temp_arg0)) != '\0'){
                if((*((volatile char *)temp_save)) != (*((char *)temp_arg0))){
                    all_same = 0;
                }
                temp_save += 0x1;
                temp_arg0 += 0x1;
            }
            if(all_same == 1 && (*((volatile char *)save_address)) == (*((char *)arg0))){
                return save_address +0x5000;
            }
        }
        save_address += 0x10000;
    }
    return -1;
}

uint32_t os_save_game(uint32_t arg0, uint32_t arg1){
    
    uint32_t save_address = SAVE_DATA;  
    
    while(save_address <= 0x70800000){
        if((*((volatile uint32_t *)save_address)) == 0){
            uint32_t save_address_half = save_address +0x5000;
            while((*((char *)arg0)) != '\0'){
                (*((volatile char *)save_address)) = (*((char *)arg0));
                save_address += 0x1;
                arg0 += 0x1;
            }
            (*((volatile char *)save_address)) = '\0';
            while((*((char *)arg1)) != '\0'){
                (*((volatile char *)save_address_half)) = (*((char *)arg1));
                save_address_half += 0x1;
                arg1 += 0x1;
            }
            (*((volatile char *)save_address_half)) = '\0';
            return 1;
        } 
        save_address += 0x10000;
    }
    return -1;
}

uint32_t create_TCB(TThreadEntry *entry, void *param){
    uint32_t OtherThreadStack[128];

    TCB *new_TCB = (TCB*)malloc(sizeof(TCB));
    new_TCB->thread_id = InitThread(OtherThreadStack + 128, *entry, param);
    new_TCB->thread_state = READY;

    return new_TCB->thread_id;
}

uint32_t c_system_call(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t call){
    if(1 == call){
        return global;
    }
    else if(2 == call){
        return controller_status;
    }
    else if(3 == call){
        return reset;
    }else if(11 == call){
        return os_save_game(arg0,arg1);
    }else if(12 == call){
        return os_get_all_saves();
    }
    else if(13 == call){
        return os_get_save(arg0);
    }
    else if(call == 9) {
        loadSprites((uint32_t*)arg0);
    }
    else if(call == 10) {
        uint16_t x = (uint16_t)(arg0>>16);
        uint16_t y = (uint16_t)(arg0&0xFFFF);
        uint16_t z = (uint16_t)(arg1>>16);
        uint16_t index = (uint16_t)(arg1&0xFFFF);
        uint16_t type = (uint16_t)(arg2>>16);
        uint16_t palette = (uint16_t)(arg2&0xFFFF);
        drawSprites(x, y, z, index, type, palette, (uint16_t)arg3);
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
        return create_TCB((TThreadEntry *) arg0, (void *)arg1);
    }
    else if(21 == call){
        SwitchThread(arg0, arg1);
        return 0;
    }

    return -1;
}




