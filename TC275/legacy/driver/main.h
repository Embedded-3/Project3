// main.h

#ifndef MAIN_H
#define MAIN_H

typedef enum {
    OPERATING = 0,
    UPDATING = 1,
} Core_Mode_t;


extern Core_Mode_t core_mode;


#endif