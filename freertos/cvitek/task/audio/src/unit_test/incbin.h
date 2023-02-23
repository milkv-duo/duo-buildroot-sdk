#ifndef __INC_BIN__
#define __INC_BIN__
#define _INCBIN(file, sym) \
    __asm__( \
        ".section .rodata\n" \
        ".global " #sym "\n" \
        /*".type " #sym ", %object\n"*/ \
        ".align 8\n" \
        #sym ":\n" \
        ".incbin \"" #file "\"\n" \
        ".size " #sym ", .-" #sym "\n" \
        ".global " #sym "_end\n" \
        ".align 8\n" \
        #sym "_end:\n" \
    );

#define INCBIN(file, sym) _INCBIN(file, sym)

#endif
