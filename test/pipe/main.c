#include "pipe.h"

int main(void) {
  int in, out;
  if(pipe_create(&in, &out) == 0) {
    /* pipe_close(in); */
    /* pipe_close(out); */
  }
  return 0;
}
