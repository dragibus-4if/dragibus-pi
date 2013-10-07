#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "process.h"

/* Switch à un autre contexte */
extern void switch_to(struct ctx_s * ctx);

#endif
