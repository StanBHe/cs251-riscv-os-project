.section .text, "ax"
.global _interrupt_handler, GetTicks, GetController, GetReset, generate_event, trigger_event, saveGame, getSaves, getSave, sys_createThread, exec, kill, wait, get_ppid, getTicks, SwitchThread
.global loadSprites, drawSpritesHelper, drawText, clearText, clearTextArea, setGraphicsMode, clearSprite

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
sys_createThread:
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
InitThread:
    li a5, 20
    ecall
SwitchThread:
    li a5, 21
    ecall
drawText:
    li a5, 22
    ecall
clearText:
    li a5, 23
    ecall
clearTextArea:
    li a5, 24
    ecall
setGraphicsMode:
    li a5, 25
    ecall
clearSprite:
    li a5, 26
    ecall