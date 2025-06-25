// Flash_Programming.c

// Include
#include <string.h>
#include "Ifx_Types.h"
#include "Flash_Programming.h"
#include "IfxFlash.h"
#include "IfxCpu.h"
#include "can.h"
#include "start.h"

// Macro
#define PFLASH_PAGE_LENGTH          IFXFLASH_PFLASH_PAGE_LENGTH /* 0x20 = 32 Bytes (smallest unit that can be
                                                                 * programmed in the Program Flash memory (PFLASH)) */
#define DFLASH_PAGE_LENGTH          IFXFLASH_DFLASH_PAGE_LENGTH /* 0x8 = 8 Bytes (smallest unit that can be
                                                                 * programmed in the Data Flash memory (DFLASH))    */
#define FLASH_MODULE                0                           /* Macro to select the flash (PMU) module           */
#define PROGRAM_FLASH_0             IfxFlash_FlashType_P0       /* Define the Program Flash Bank to be used         */
#define DATA_FLASH_0                IfxFlash_FlashType_D0       /* Define the Data Flash Bank to be used            */

//#define DATA_TO_WRITE               0x10000000 //0x01000000 //0x4F7403DA //0xDA02744F //0x04DA9000 //0x0090DA04 //0x07738135                  /* Dummy data to be written in the Flash memories   */



// base address

//#define DFLASH_STARTING_ADDRESS     (0xaf000000)

// ota start address
//#define PFLASH_STARTING_ADDRESS     ((*DFLASH_BASE_ADDRESS == 0x00000001) ? PFLASH_PARTITION1 : PFLASH_PARTITION0)


//#define PFLASH_STARTING_ADDRESS     0xA000008E //0xA0000098 //0xA00E0000 //0x60000000 // 0x800015F8 //0xA00E0000                  /* Address of the PFLASH where the data is written  */
//#define DFLASH_STARTING_ADDRESS     0xAF000000 //0x60000000  //0xAF000000                  /* Address of the DFLASH where the data is written  */



#define PFLASH_NUM_PAGE_TO_FLASH    1                           /* Number of pages to flash in the PFLASH           */
#define PFLASH_NUM_SECTORS          1                           /* Number of PFLASH sectors to be erased            */
#define DFLASH_NUM_PAGE_TO_FLASH    8                           /* Number of pages to flash in the DFLASH           */
#define DFLASH_NUM_SECTORS          1                           /* Number of DFLASH sectors to be erased            */

/* Reserved space for erase and program routines in bytes */
#define ERASESECTOR_LEN             (100)
#define WAITUNBUSY_LEN              (100)
#define ENTERPAGEMODE_LEN           (100)
#define LOADPAGE2X32_LEN            (100)
#define WRITEPAGE_LEN               (100)
#define ERASEPFLASH_LEN             (0x100)
#define WRITEPFLASH_LEN             (0x200)

/* Relocation address for the erase and program routines: Program Scratch-Pad SRAM (PSPR) of CPU0 */
#define RELOCATION_START_ADDR       (0x70100000U)   // RAM 상의 주소

/* Definition of the addresses where to relocate the erase and program routines, given their reserved space */
#define ERASESECTOR_ADDR            (RELOCATION_START_ADDR)
#define WAITUNBUSY_ADDR             (ERASESECTOR_ADDR + ERASESECTOR_LEN)
#define ENTERPAGEMODE_ADDR          (WAITUNBUSY_ADDR + WAITUNBUSY_LEN)
#define LOAD2X32_ADDR               (ENTERPAGEMODE_ADDR + ENTERPAGEMODE_LEN)
#define WRITEPAGE_ADDR              (LOAD2X32_ADDR + LOADPAGE2X32_LEN)
#define ERASEPFLASH_ADDR            (WRITEPAGE_ADDR + WRITEPAGE_LEN)
#define WRITEPFLASH_ADDR            (ERASEPFLASH_ADDR + ERASEPFLASH_LEN)

/* Definition of the LEDs port pins */
#define LED1                        &MODULE_P10,1               /* LED connected to Port 00, Pin 5                  */
#define LED2                        &MODULE_P10,2               /* LED connected to Port 00, Pin 6                  */

#define MEM(address)                *((uint32 *)(address))      /* Macro to simplify the access to a memory address */


// Function Prototype
void erasePFLASH(uint32 sectorAddr);
void writePFLASH(uint32 startingAddr);
void copyFunctionsToPSPR(void);

typedef struct
{
    void (*eraseSectors)(uint32 sectorAddr, uint32 numSector);
    uint8 (*waitUnbusy)(uint32 flash, IfxFlash_FlashType flashType);
    uint8 (*enterPageMode)(uint32 pageAddr);
    void (*load2X32bits)(uint32 pageAddr, uint32 wordL, uint32 wordU);
    void (*writePage)(uint32 pageAddr);
    void (*eraseFlash)(uint32 sectorAddr);
    void (*writeFlash)(uint32 startingAddr);
} Function;


// global
Function g_commandFromPSPR;


// Function Implementations
/* This function copies the erase and program routines to the Program Scratch-Pad SRAM (PSPR) of the CPU0 and assigns
 * function pointers to them.
 */
void copyFunctionsToPSPR()
{
    /* Copy the IfxFlash_eraseMultipleSectors() routine and assign it to a function pointer */
    memcpy((void *)ERASESECTOR_ADDR, (const void *)IfxFlash_eraseMultipleSectors, ERASESECTOR_LEN);
    g_commandFromPSPR.eraseSectors = (void *)ERASESECTOR_ADDR;

    /* Copy the IfxFlash_waitUnbusy() routine and assign it to a function pointer */
    memcpy((void *)WAITUNBUSY_ADDR, (const void *)IfxFlash_waitUnbusy, WAITUNBUSY_LEN);
    g_commandFromPSPR.waitUnbusy = (void *)WAITUNBUSY_ADDR;

    /* Copy the IfxFlash_enterPageMode() routine and assign it to a function pointer */
    memcpy((void *)ENTERPAGEMODE_ADDR, (const void *)IfxFlash_enterPageMode, ENTERPAGEMODE_LEN);
    g_commandFromPSPR.enterPageMode = (void *)ENTERPAGEMODE_ADDR;

    /* Copy the IfxFlash_loadPage2X32() routine and assign it to a function pointer */
    memcpy((void *)LOAD2X32_ADDR, (const void *)IfxFlash_loadPage2X32, LOADPAGE2X32_LEN);
    g_commandFromPSPR.load2X32bits = (void *)LOAD2X32_ADDR;

    /* Copy the IfxFlash_writePage() routine and assign it to a function pointer */
    memcpy((void *)WRITEPAGE_ADDR, (const void *)IfxFlash_writePage, WRITEPAGE_LEN);
    g_commandFromPSPR.writePage = (void *)WRITEPAGE_ADDR;

    /* Copy the erasePFLASH() routine and assign it to a function pointer */
    memcpy((void *)ERASEPFLASH_ADDR, (const void *)erasePFLASH, ERASEPFLASH_LEN);
    g_commandFromPSPR.eraseFlash = (void *)ERASEPFLASH_ADDR;

    /* Copy the erasePFLASH() routine and assign it to a function pointer */
    memcpy((void *)WRITEPFLASH_ADDR, (const void *)writePFLASH, WRITEPFLASH_LEN);
    g_commandFromPSPR.writeFlash = (void *)WRITEPFLASH_ADDR;
}

/* This function erases a given sector of the Program Flash memory. The function is copied in the PSPR through
 * copyFunctionsToPSPR(). Because of this, inside the function, only routines from the PSPR or inline functions
 * can be called, otherwise a Context Type (CTYP) trap can be triggered.
 */
void erasePFLASH(uint32 sectorAddr)
{
    /* Get the current password of the Safety WatchDog module */
    uint16 endInitSafetyPassword = IfxScuWdt_getSafetyWatchdogPasswordInline();

    /* Erase the sector */
    IfxScuWdt_clearSafetyEndinitInline(endInitSafetyPassword);      /* Disable EndInit protection                   */
    g_commandFromPSPR.eraseSectors(sectorAddr, PFLASH_NUM_SECTORS); /* Erase the given sector                       */
    IfxScuWdt_setSafetyEndinitInline(endInitSafetyPassword);        /* Enable EndInit protection                    */

    /* Wait until the sector is erased */
    g_commandFromPSPR.waitUnbusy(FLASH_MODULE, PROGRAM_FLASH_0);
}

/* This function writes the Program Flash memory. The function is copied in the PSPR through copyFunctionsToPSPR().
 * Because of this, inside the function, only routines from the PSPR or inline functions can be called,
 * otherwise a Context Type (CTYP) trap can be triggered.
 */
void writePFLASH(uint32 startingAddr)
{
    uint32 page;                                                /* Variable to cycle over all the pages             */
    uint32 offset;                                              /* Variable to cycle over all the words in a page   */
    uint32 errors = 0;
    /* Get the current password of the Safety WatchDog module */
    uint16 endInitSafetyPassword = IfxScuWdt_getSafetyWatchdogPasswordInline();

    /* Write all the pages */
    for(page = 0; page < fwPage; page++)               //Loop over all the pages
    //for(page = 0; page < PFLASH_NUM_PAGE_TO_FLASH; page++)              /* Loop over all the pages                  */
    {
        uint32 pageAddr = startingAddr + (page * PFLASH_PAGE_LENGTH);   /* Get the address of the page              */
        /* Enter in page mode */
        g_commandFromPSPR.enterPageMode(pageAddr);

        /* Wait until page mode is entered */
        g_commandFromPSPR.waitUnbusy(FLASH_MODULE, PROGRAM_FLASH_0);

        /* Write 32 bytes (8 double words) into the assembly buffer */
        for (offset = 0; offset < PFLASH_PAGE_LENGTH; offset += 0x8)
        {
            uint32 index = offset / 0x8 * 2;  // 0,2,4,6

            g_commandFromPSPR.load2X32bits(pageAddr + offset, firmware_data[page][index], firmware_data[page][index + 1]);
        }

        /* Write the page */
        IfxScuWdt_clearSafetyEndinitInline(endInitSafetyPassword);      /* Disable EndInit protection               */
        g_commandFromPSPR.writePage(pageAddr);                          /* Write the page                           */
        IfxScuWdt_setSafetyEndinitInline(endInitSafetyPassword);        /* Enable EndInit protection                */

        /* Wait until the page is written in the Program Flash memory */
        g_commandFromPSPR.waitUnbusy(FLASH_MODULE, PROGRAM_FLASH_0);

        //  verify
        uint32 check = 0;
        // from 0 to 8
        for(offset = 0; offset < PFLASH_PAGE_LENGTH; offset += 0x4)                 /* Loop over the page length    */
        {
            /* Check if the data in the Program Flash is correct */
            if(MEM(pageAddr + offset) != firmware_data[page][check++])
            {
                /* If not, count the found errors */
                errors++;
                /* If error, Try again*/
//                IfxScuWdt_clearSafetyEndinitInline(endInitSafetyPassword);      /* Disable EndInit protection               */
//                g_commandFromPSPR.writePage(pageAddr);                          /* Write the page                           */
//                IfxScuWdt_setSafetyEndinitInline(endInitSafetyPassword);        /* Enable EndInit protection                */
            }
        }
//        if(errors == 0)
//        {
//            IfxPort_setPinState(LED1, IfxPort_State_high);
//        }
        check = 0;


    }
}

/* This function flashes the Program Flash memory calling the routines from the PSPR */
void writeProgramFlash()
{
    boolean interruptState = IfxCpu_disableInterrupts(); /* Get the current state of the interrupts and disable them*/

    /* Copy all the needed functions to the PSPR memory to avoid overwriting them during the flash execution */
    copyFunctionsToPSPR();

    /* Erase the Program Flash sector before writing */
    g_commandFromPSPR.eraseFlash(PFLASH_STARTING_ADDRESS);

    /* Write the Program Flash */
    g_commandFromPSPR.writeFlash(PFLASH_STARTING_ADDRESS);

    IfxCpu_restoreInterrupts(interruptState);            /* Restore the interrupts state                            */

    // 1. 플래싱 후 파랑 led on
    // 2. 데이터 플래시에도 작성
    // 3. 소프트웨어 재부팅
    IfxPort_setPinState(LED2, IfxPort_State_high);
    writeDataFlash();
    for(volatile int t = 0;t<30000000;t++);

    uint16 password = IfxScuWdt_getSafetyWatchdogPasswordInline();
    IfxScuWdt_clearSafetyEndinitInline(password);
    //MODULE_SCU.SWRSTCON.B.SWRSTREQ = 1;
    IfxScuWdt_setSafetyEndinitInline(password);

    //while (1) {}
}

/* This function verifies if the data has been correctly written in the Program Flash */
void verifyProgramFlash()
{
    uint32 page;                                                /* Variable to cycle over all the pages             */
    uint32 offset;                                              /* Variable to cycle over all the words in a page   */
    uint32 errors = 0;                                          /* Variable to keep record of the errors            */

    /* Verify the written data */
    for(page = 0; page < PFLASH_NUM_PAGE_TO_FLASH; page++)                          /* Loop over all the pages      */
    {
        uint32 pageAddr = PFLASH_STARTING_ADDRESS + (page * PFLASH_PAGE_LENGTH);    /* Get the address of the page  */

        for(offset = 0; offset < PFLASH_PAGE_LENGTH; offset += 0x4)                 /* Loop over the page length    */
        {
            /* Check if the data in the Program Flash is correct */
            if(MEM(pageAddr + offset) == DATA_TO_WRITE)
            {
                /* If not, count the found errors */
                errors++;
            }
        }
    }

    /* If the data is correct, turn on the LED1 */
    if(errors == 0)
    {
        ;
        //IfxPort_setPinState(LED1, IfxPort_State_low);
    }
}



/* This function flashes the Data Flash memory.
 * It is not needed to run this function from the PSPR, thus it can call functions from the Program Flash memory.
 */
void writeDataFlash()
{
    uint32 page;                                                /* Variable to cycle over all the pages             */

    /* --------------- ERASE PROCESS --------------- */
    /* Get the current password of the Safety WatchDog module */
    uint16 endInitSafetyPassword = IfxScuWdt_getSafetyWatchdogPassword();

    /* Erase the sector */
    IfxScuWdt_clearSafetyEndinit(endInitSafetyPassword);        /* Disable EndInit protection                       */
    IfxFlash_eraseMultipleSectors(DFLASH_STARTING_ADDRESS, DFLASH_NUM_SECTORS); /* Erase the given sector           */
    IfxScuWdt_setSafetyEndinit(endInitSafetyPassword);          /* Enable EndInit protection                        */

    /* Wait until the sector is erased */
    IfxFlash_waitUnbusy(FLASH_MODULE, DATA_FLASH_0);

    /* --------------- WRITE PROCESS --------------- */
    for(page = 0; page < DFLASH_NUM_PAGE_TO_FLASH; page++)      /* Loop over all the pages                          */
    {
        uint32 pageAddr = DFLASH_STARTING_ADDRESS + (page * DFLASH_PAGE_LENGTH); /* Get the address of the page     */

        /* Enter in page mode */
        IfxFlash_enterPageMode(pageAddr);

        /* Wait until page mode is entered */
        IfxFlash_waitUnbusy(FLASH_MODULE, DATA_FLASH_0);

        /* Load data to be written in the page */
        IfxFlash_loadPage2X32(pageAddr, DATA_TO_WRITE, 0x00000000); /* Load two words of 32 bits each            */

        /* Write the loaded page */
        IfxScuWdt_clearSafetyEndinit(endInitSafetyPassword);    /* Disable EndInit protection                       */
        IfxFlash_writePage(pageAddr);                           /* Write the page                                   */
        IfxScuWdt_setSafetyEndinit(endInitSafetyPassword);      /* Enable EndInit protection                        */

        /* Wait until the data is written in the Data Flash memory */
        IfxFlash_waitUnbusy(FLASH_MODULE, DATA_FLASH_0);
        break;
    }
}

/* This function verifies if the data has been correctly written in the Data Flash */
// void verifyDataFlash()
// {
//     uint32 page;                                                /* Variable to cycle over all the pages             */
//     uint32 offset;                                              /* Variable to cycle over all the words in a page   */
//     uint32 errors = 0;                                          /* Variable to keep record of the errors            */

//     /* Verify the written data */
//     for(page = 0; page < DFLASH_NUM_PAGE_TO_FLASH; page++)                          /* Loop over all the pages      */
//     {
//         uint32 pageAddr = DFLASH_STARTING_ADDRESS + (page * DFLASH_PAGE_LENGTH);    /* Get the address of the page  */

//         for(offset = 0; offset < DFLASH_PAGE_LENGTH; offset += 0x4)                 /* Loop over the page length    */
//         {
//             /* Check if the data in the Data Flash is correct */
//             if(MEM(pageAddr + offset) != DATA_TO_WRITE)
//             {
//                 /* If not, count the found errors */
//                 errors++;
//             }
//         }
//     }

//     /* If the data is correct, turn on the LED2 */
//     if(errors == 0)
//     {
//         ;
//         //IfxPort_setPinState(LED2, IfxPort_State_low);
//     }
// }
