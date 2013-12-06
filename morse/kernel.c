#include "../os/morse.h"
#include "../os/malloc.h"
#include "../os/hw.h"

int start_kernel(void) {
    /* Initialisation de la RAM */
    malloc_init((void *) HEAP_START);

    morse_blink_buffer("Hello World", 11);

    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
