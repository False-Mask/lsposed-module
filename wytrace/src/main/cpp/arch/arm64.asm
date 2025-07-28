section .text
global executeNTerpImpl_proxy
extern NterpBeforeExecute
extern NterpAfterExecute
extern originCallAddr

executeNTerpImpl_proxy:
    // saveAll
    sub sp, sp, #80
    stp x0, x1, [sp, #0]
    stp x2, x3, [sp, #16]
    stp x4, x5, [sp, #32]
    stp x6, x7, [sp, #48]
    str x8, [sp, #64]
    bl NterpBeforeExecute

    // restore
    ldr x8, [sp, #64]
    ldp x6, x7, [sp, #48]
    ldp x4, x5, [sp, #32]
    ldp x2, x3, [sp, #16]
    ldp x0, x1, [sp, #0]
    bl originCallAddr
    add sp, sp, #72
    ldp x0, x1, [sp, #0]
    bl NterpAfterExecute
    ret







