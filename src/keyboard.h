#ifndef KEYBOARD_H
#define KEYBOARD_H

//http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/input01.html
// TODO : includer les fichiers nécessaires


#include<usbd/usbd.h>
#include<device/hid/keyboard.h>
#include<types.h>
#include<../../hw.h>

// initialise le driver, commence à écouter les entrées clavier
void start_driver();


//

#endif
