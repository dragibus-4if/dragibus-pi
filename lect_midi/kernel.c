#include "../os/hw.h"
#include "../os/sched.h"
#include "../os/malloc.h"
#include "../os/sem.h"
#include "../os/mutex.h"
#include "music/music.h"

//------------------------------------------------------------------------
int start_kernel(void) {
    init_hw();
    music_init();
    malloc_init((void *) HEAP_START);
    create_process(play_music, NULL, 2048);
    start_sched();
    return 0;
}
