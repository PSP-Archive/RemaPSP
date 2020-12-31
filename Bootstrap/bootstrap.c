
/*

  Bootstrap for 2.00 Game Loader based on bootstrap loader of psplink by Tyranid

  by mph (mphtheone@hotmail.com)

  Thanks to Tyranid for his wonderful psplink tool, psppet for his psardump tool and all pspsdk team

*/


// *** INCLUDES ***

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <pspdisplay.h>
#include <stdio.h>
#include <string.h>
#include <pspctrl.h>

#include <pspsdk.h>


//#define printf					pspDebugScreenPrintf


// *** MODULE INITIALISATION ***

PSP_MODULE_INFO("BootstrapLoader",0x1000,1,1);
PSP_MAIN_THREAD_ATTR(0x0);


// *** FUNCTIONS DECLARATIONS ***

SceUID loadModule (const char *, int, int);


// *** FUNCTIONS CALLBACKS ***

int CallbackExit (int arg1, int arg2, void *common)
{
 sceKernelExitGame();

 return 0;
}

int CallbackThread (SceSize args, void *argp)
{
 int cbid;


 cbid = sceKernelCreateCallback("ExitCallback",CallbackExit,NULL);
 sceKernelRegisterExitCallback(cbid);

 sceKernelSleepThreadCB();

 return 0;
}

int SetupCallbacks (void)
{
 int thid;


 thid = sceKernelCreateThread("UpdateThread",CallbackThread,0x11,0xFA0,PSP_THREAD_ATTR_USER,0);
 if (thid >= 0) sceKernelStartThread(thid,0,0);

 return thid;
}


// *** FUNCTIONS ***

SceUID loadModule (const char *path, int flags, int type)
{
 SceKernelLMOption option;
 SceUID mpid;


 // If the type is 0, then load the module in the kernel partition, otherwise load it in the user partition.
 mpid = (type) ? 2 : 1;

 // Initialize structure
 memset(&option,0,sizeof(option));
 option.size = sizeof(option);
 option.mpidtext = mpid;
 option.mpiddata = mpid;
 option.position = 0;
 option.access = 1;

 return sceKernelLoadModule(path,flags,(type) ? &option : NULL);
}

int MainThread (SceSize args, void *argp)
{
	SceUID modid;
	char args2[16];
	
		//pspKernelSetKernelPC();
	pspSdkInstallNoDeviceCheckPatch();
	pspSdkInstallNoPlainModuleCheckPatch();

	//NEW
	pspSdkInstallKernelLoadModulePatch();

	// Init display and HOME button
	pspDebugScreenInit();
	pspDebugScreenClear();
	SetupCallbacks();
	
	// pspDebugInstallKprintfHandler(NULL);			// Bug when placed
	
	// Create args
	sprintf(args2,"%08X",sceKernelGetThreadId());
	
	// Call program
	printf("RemaPSP by danzel\n");
	printf("Loading Modules ...\n");
	
	
//	#if 0
	modid = loadModule("host1:/nkthread.prx",0,0);
//	modid = loadModule("ms0:/remaps/prx/nkthread.prx",0,0);
	sceKernelStartModule(modid,strlen(args2) + 1,args2,NULL,NULL);
	
	modid = loadModule("host1:/remapsp.prx",0,0);
//	modid = loadModule("ms0:/remaps/prx/remapsp.prx",0,0);
	sceKernelStartModule(modid,strlen(args2) + 1,args2,NULL,NULL);
//#endif	
	// For somes game that don't erase screen
	//pspDebugScreenClear();
	
	// Exit
	sceKernelExitDeleteThread(0);
	
	return 0;
}

// Entry point

int module_start(SceSize args, void *argp) __attribute__((alias("_start")));

int _start (SceSize args, void *argp)
{
	SceUID thid;
	u32 func;
	
	
	func = (u32) MainThread;
	func |= 0x80000000;
	
	thid = sceKernelCreateThread("MainThread",(void *) func,0x20,0x10000,0,NULL);
	if (thid >= 0) sceKernelStartThread(thid,args,argp);
	
	
	return 0;
}

int module_stop (void)
{
 return 0;
}
