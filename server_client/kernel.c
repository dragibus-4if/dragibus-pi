#include "../os/pipe.h"
#include "../os/malloc.h"
#include "../os/hw.h"
#include "../os/sched.h"

int pipe_write_all(pipe_t p, const char * buffer, size_t bufsize) {
    for(ssize_t ws = 0 ; ws < bufsize ;) {
        ssize_t w = pipe_write(p, buffer + ws, bufsize - ws);
        if (w == -1) {
            return -1;
        }
        ws += w;
    }
    return 0;
}

int pipe_read_all(pipe_t p, char * buffer, size_t bufsize) {
    for(ssize_t rs = 0 ; rs < bufsize ;) {
        ssize_t r = pipe_read(p, buffer + rs, bufsize - rs);
        if (r == -1) {
            return -1;
        }
        rs += r;
    }
    return 0;
}

void server(void * writable) {
    pipe_t output = *(pipe_t *) writable;
    unsigned long long int data = 42;
    while(1) {
        if(pipe_write_all(output, (char *) &data, sizeof(data)) != -1) {
            data++;
        }
    }
}

void client(void * readable) {
    pipe_t input = *(pipe_t *) readable;
    unsigned long long int data;
    while(1) {
        pipe_read_all(input, (char *) &data, sizeof(data));
    }
}

int start_kernel(void) {
    /* Initialisation de la RAM */
    DISABLE_IRQ();
    init_hw();
    malloc_init((void *) HEAP_START);
    pipe_t input, output;
    if(pipe_create(&input, &output) != -1) {
        create_process(&server, (void *) &output, 4096, SCHED_OTHER, 5);
        create_process(&client, (void *) &input, 4096, SCHED_OTHER, 5);
        sched_start();
    }

    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
