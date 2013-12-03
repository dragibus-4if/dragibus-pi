//
//  mutex.h
//  
//
//  Created by Peller Patrizia on 03/12/2013.
//
//

#ifndef MUTEX_H
#define MUTEX_H

int mutex_create(int * desc);

int mutex_destroy(int desc);

int mutex_acquire(int desc);

int mutex_release(int desc);

#endif
