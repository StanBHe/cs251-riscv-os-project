.section .text, "ax"
.global _interrupt_handler, GetTicks, GetController, GetReset, generate_event, trigger_event, malloc, realloc, free, loadSprites, drawSpritesHelper, saveGame, getSaves, getSave, fork, exec, kill, wait, get_ppid, getTicks

_interrupt_handler:
    addi	sp,sp,-40
    sw	    ra,36(sp)
    sw	    t0,32(sp)
    sw	    t1,28(sp)
    sw	    t2,24(sp)
    sw	    a0,20(sp)
    sw	    a1,16(sp)
    sw	    a2,12(sp)
    sw	    a3,8(sp)
    sw	    a4,4(sp)
    sw	    a5,0(sp)
    call    c_interrupt_handler
    lw	    ra,36(sp)
    lw	    t0,32(sp)
    lw	    t1,28(sp)
    lw	    t2,24(sp)
    lw	    a0,20(sp)
    lw	    a1,16(sp)
    lw	    a2,12(sp)
    lw	    a3,8(sp)
    lw	    a4,4(sp)
    lw	    a5,0(sp)
    addi    sp,sp,40
    mret

GetTicks:
    li a5, 1
    ecall
GetController:
    li a5, 2
    ecall
GetReset:
    li a5, 3
    ecall
generate_event:
    li a5, 4
    ecall
trigger_event:
    li a5, 5
    ecall
malloc:
    li a5, 6
    ecall
realloc:
    li a5, 7
    ecall
free:
    li a5, 8
    ecall
loadSprites:
    li a5, 9
    ecall
drawSpritesHelper:
    li a5, 10
    ecall
saveGame:
    li a5, 11
    ecall
getSaves:
    li a5, 12
    ecall
getSave:
    li a5, 13
    ecall
fork:
    li a5, 14
    ecall
exec:
    li a5, 15
    ecall
kill:
    li a5, 16
    ecall
wait:
    li a5, 17
    ecall
get_ppid:
    li a5, 18
    ecall
getTicks:
    li a5, 19
    ecall


