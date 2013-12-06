#include "mutex.h"
#include "malloc.h"

struct mutex_s {
};

struct mutex_s * mutex_create() {
    return (struct mutex_s *)
      malloc_alloc(sizeof(struct mutex_s));
}

int mutex_free(struct mutex_s * mutex) {
    malloc_free((void *) mutex);
    return 0;
}

int mutex_init(struct mutex_s * mutex) {
    return 0;
}

int mutex_acquire(struct mutex_s * mutex) {
    return 0;
}

int mutex_release(struct mutex_s * mutex) {
    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
