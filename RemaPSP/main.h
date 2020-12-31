
#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

/*
	RemaPSP by danzel (danzelatlocalhostdotgeekdotnzkthx)

	Based on 2.00 Game Loader for 1.50 by mph, thanks man :)
*/


// *** INCLUDES ***

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspumd.h>
#include <stdio.h>
#include <stdlib.h>

#include "conf.h"
#include "Utils/module.h"

extern SceUID sceKernelSearchModuleByName (const char *);

//Global for use with the menu :)
extern struct ctrlSetup controllerConfig;

// *** DEFINES ***

//Define only one of these:
//#define BUILD_DEVHOOK
//#define LOADING_UMD
//#define LOADING_HB
#define BUILD_MENU

//And for release
//#define NODEBUG

//Get crazy wifi hax too~
//#define ENABLE_WIFI



#ifdef NODEBUG
#define printf(...); ;
#endif


#define	DEBUG_HOOK				0
#define	DEBUG_DRIVER			0

#define	MAIN_LOADER_NAME		"BootstrapLoader"

#define	MAIN_FOLDER_MAIN		"mphgameloader"
#define	MAIN_FOLDER_FLASH0		"flash0"
#define	MAIN_FOLDER_FLASH1		"flash1"
#define	MAIN_FOLDER_GAME		"game"

#define	MAIN_UMDFILE_ID			"disc0:/UMD_DATA.BIN"
#define	MAIN_UMDFILE_EXEC		"disc0:/PSP_GAME/SYSDIR/BOOT.BIN"


// *** MACROS ***

#if (DEBUG_DRIVER != 0)

#define	DEBUG_DRIVER_PATH	"ms0:/mphgl_driver.log"

#else

#define	DEBUG_DRIVER_PATH	NULL

#endif

#if (DEBUG_HOOK != 0)

#define HOOK_LOGFILE_INIT { int _fd = sceIoOpen("ms0:/mphgl_patch.log",PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC,0777); sceIoClose(_fd); }
#define HOOK_LOGFILE_WRITE(fmt, ...)											\
{																				\
 char _string[256];																\
 int _fd = sceIoOpen("ms0:/mphgl_patch.log",PSP_O_WRONLY | PSP_O_APPEND,0777);	\
 sprintf(_string,fmt,## __VA_ARGS__);											\
 sceIoWrite(_fd,_string,strlen(_string));										\
 sceIoClose(_fd);																\
}

#else

#define	HOOK_LOGFILE_INIT
#define	HOOK_LOGFILE_WRITE(fmt, ...)

#endif


typedef struct MainHook
{
 ModuleFunc modfunc;
 char modname[32];
 char libname[32];
 u32 nid;
 void *func;
} MainHook;

// *** FUNCTIONS DECLARATIONS ***

int sceCtrlPeekBufferPositiveFake(SceCtrlData *pad_data, int count);
int sceCtrlPeekBufferNegativeFake(SceCtrlData *pad_data, int count);
int sceCtrlReadBufferPositiveFake(SceCtrlData *pad_data, int count);
int sceCtrlReadBufferNegativeFake(SceCtrlData *pad_data, int count);
//int vshCtrlReadBufferPositiveFake(SceCtrlData *pad_data, int count);

int sceCtrlPeekLatchFake(SceCtrlLatch *latch_data);
int sceCtrlReadLatchFake(SceCtrlLatch *latch_data);

extern unsigned char enabled;
#endif
