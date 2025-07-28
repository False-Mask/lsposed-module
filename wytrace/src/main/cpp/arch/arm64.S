#include "asm.h"

.extern executeNterpImpl_orgi

//.data
//.globl shared_data
//shared_data:
//dcq 0x0

ENTRY_GLOBAL_ARM(executeNTerpImpl_proxy)
    // saveAll
    sub sp, sp, #80
    stp x0, x1, [sp, #0]
    stp x2, x3, [sp, #16]
    stp x4, x5, [sp, #32]
    stp x6, x7, [sp, #48]
    str x8, [sp, #64]
    mov x18, x30

    // restore
    ldr x8, [sp, #64]
    ldp x6, x7, [sp, #48]
    ldp x4, x5, [sp, #32]
    ldp x2, x3, [sp, #16]
    ldp x0, x1, [sp, #0]
    add sp, sp, #80
    adrp x16, :got:executeNterpImpl_orgi
    ldr x16,[x16, :got_lo12:executeNterpImpl_orgi]
    ldr x16,[x16]
//    br x16
    blr x16
    mov x30, x18
    ret x30
END(executeNTerpImpl_proxy)






