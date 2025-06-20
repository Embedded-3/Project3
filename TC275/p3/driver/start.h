
#ifndef START_H
#define START_H

#include "Ifx_Types.h"
#include "Platform_Types.h"
#include "main.h"

#define DFLASH_STARTING_ADDRESS     (0xaf000000)

typedef struct {
    void (*ota)(void);
} FuncPtr;


extern uint32 PFLASH_STARTING_ADDRESS;
extern uint32 DATA_TO_WRITE;


extern FuncPtr g_funcptr;
extern Core_Mode_t core_mode;

//extern int abc;



//void software_init_hook();
// void ota0();
// void ota1();


#endif
