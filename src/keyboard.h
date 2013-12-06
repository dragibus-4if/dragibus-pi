#ifndef KEYBOARD_H
#define KEYBOARD_H

//http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/input01.html
// TODO : includer les fichiers nécessaires
//TODO : copycat l'exemple du lien ci dessu en c, en utilisant direct- les fonctions de la librairie csud

#include<csud-master/include/usbd/usbd.h>
#include<csud-master/include/device/hid/keyboard.h>
#include<csud-master/include/types.h>
#include<stdio.h>

// initialise le driver, commence à écouter les entrées clavier
public void start_driver();


//

#endif
