#include "hw.h"
#include "music.h"
#include "yield.h"

extern void play_music();

//------

void
processus_A( void* args )
{
  int cptA = 0;

  while ( 1 ) {
    cptA ++;
  }
}

void
processus_B( void* args )
{
  int cptB = 1;

  while ( 1 ) {
    cptB += 2 ;
  }
}


//------------------------------------------------------------------------
int
notmain ( void )
{
  hw_init();
  music_init();

  /* hw_init(); */
  /* led_init(); */
  /* switch_init(); */

  /* play_music(); */
  create_process(processus_A, NULL, 128);
  create_process(processus_A, NULL, 128);

  create_process(play_music, NULL, 2048);

  create_process(processus_B, NULL, 128);
  create_process(processus_B, NULL, 128);

  start_sched();

  return(0);
}

