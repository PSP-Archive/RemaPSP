/*
	RemaPSP by danzel (danzelatlocalhostdotgeekdotnzkthx)
	
	Based on 2.00 Game Loader for 1.50 by mph, thanks man :)
*/

#include <pspctrl.h>
#include <psphprm.h>
#include <psppower.h>

#include <time.h>
#include <psprtc.h>

#include "main.h"
#include "hooktable.h"
#include "wifi.h"
#include "menu.h"
#include "launchmenu.h"

char mainUmdId[16];

// *** MODULE INITIALISATION ***
PSP_MODULE_INFO("RemaPSP",0x1000,1,1);

// *** FUNCTIONS PATCH ***
typedef int (*FUNC_CTRL) (SceCtrlData*, int);
typedef int (*FUNC_LATCH) (SceCtrlLatch*);

#define PAD_THRESHOLD 50

SceCtrlData dupe_pad;
struct ctrlSetup controllerConfig;

unsigned char holdingNote = 0;
unsigned char enabled    = 0;
unsigned char inTurbo = 0; //Toggle every time we are called

//Macro Variables
unsigned char inMacro = 0; //0 if not in a macro, 1/2 if we are - index of macro + 1
u64 macroStartTime = 0; //Time the current macro piece started
unsigned char macroUpToIndex; //Index of current Macro Piece

//Works out how far an analog is pressed for remapping the analogs
//This returns the difference from center that the given control is, based on dupe_pad
unsigned char getAnalog(const u16 control)
{
	switch(control)
	{
	case CTRL_ANALOG_UP:
		return (dupe_pad.Ly <127)?127-dupe_pad.Ly:0;
	case CTRL_ANALOG_RIGHT:
		return (dupe_pad.Lx >128)?dupe_pad.Lx-128:0;
	case CTRL_ANALOG_DOWN:
		return (dupe_pad.Ly >128)?dupe_pad.Ly-128:0;
	case CTRL_ANALOG_LEFT:
		return (dupe_pad.Lx <127)?127-dupe_pad.Lx:0;
	
	default: //none are set
		return 0;
	}
}


//1000*1000 per second

void runMacro(SceCtrlData *pad_data)
{
	//Check if current Macro line has expired
	u64 timeDifference;
	sceRtcGetCurrentTick(&timeDifference);
	timeDifference -= macroStartTime;
	if (timeDifference >= Macro[inMacro-1].time[macroUpToIndex]) //Next if needed
	{
		macroUpToIndex++;
		printf("Moving to part %i\n", macroUpToIndex);
		sceRtcGetCurrentTick(&macroStartTime);
	}
	
	if (macroUpToIndex > Macro[inMacro-1].size) //Check if macro is done
	{
		printf("Macro Finished\n");
		inMacro = 0;
		runMenu = MENU_DO_NOTHING;
		return;
	}
	
	//TODO ANALOG, This does digitals!
	pad_data->Buttons |= Macro[inMacro-1].buttons[macroUpToIndex].pressed | ((inTurbo)?Macro[inMacro-1].buttons[macroUpToIndex].turbo:0);
}

void overrideControls(SceCtrlData *pad_data)
{
	memcpy(&dupe_pad, pad_data, sizeof(SceCtrlData));
	
	if ((dupe_pad.Buttons & PSP_CTRL_VOLUP) && (dupe_pad.Buttons & PSP_CTRL_VOLDOWN))
	{
		runMenu = 1;
		while (runMenu)
			sceKernelDelayThread(1000*500);
	}
	
	if (!(dupe_pad.Buttons & PSP_CTRL_NOTE))
		holdingNote = 0;
	
	if (enabled)
	{
		///Set up the variables for processing
		inTurbo = !inTurbo; //Toggle the turbos~
		
		//Clear off the area for the remote, some useless bits up there
		dupe_pad.Buttons &= 0x00ffffff;
		
		//Set up some extra bits in the dupe_pad.Buttons (remote and analog)
		if (sceHprmIsRemoteExist()) ///Remote
		{
			u32 hprm = 0;
			sceHprmPeekCurrentKey(&hprm);
			dupe_pad.Buttons |= (hprm<<24);
		}
		
		//Set the bits used for analogs if required
		     if (dupe_pad.Lx < PAD_THRESHOLD)       dupe_pad.Buttons |= CTRL_ANALOG_LEFT;
		else if (dupe_pad.Lx > (255-PAD_THRESHOLD)) dupe_pad.Buttons |= CTRL_ANALOG_RIGHT;
		
		     if (dupe_pad.Ly < PAD_THRESHOLD)       dupe_pad.Buttons |= CTRL_ANALOG_UP;
		else if (dupe_pad.Ly > (255-PAD_THRESHOLD)) dupe_pad.Buttons |= CTRL_ANALOG_DOWN;
		
		//Set some defaults: Center the analog, 0 the buttons
		pad_data->Lx = 127; pad_data->Ly = 127;
		pad_data->Buttons = 0;
		
		if (!inMacro)
		{
			if (Macro[0].trigger.pressed != 0 && (dupe_pad.Buttons & Macro[0].trigger.pressed) == Macro[0].trigger.pressed && Macro[0].runYet)
			{
				inMacro = 1;
			}
			else if (Macro[1].trigger.pressed != 0 && (dupe_pad.Buttons & Macro[1].trigger.pressed) == Macro[1].trigger.pressed && Macro[1].runYet)
			{
				inMacro = 2;
			}
			
			if (inMacro)
			{
				printf("Into Macro %i\n", inMacro);
				Macro[inMacro-1].runYet = (Macro[inMacro-1].runYet == 1)?0:2; //set to 0 if 1.
				macroUpToIndex = 0;
				sceRtcGetCurrentTick(&macroStartTime);
				sprintf(menuPrintText, "MACRO %i", inMacro);
			}
		}
		
		if (inMacro)
		{
			runMenu = MENU_PRINT_TEXT;
			runMacro(pad_data);
			if (pad_data->Buttons) //if there are any keys set, send a power stay on!
				scePowerTick(0);
			return;
		}
		
		///Now create the new overrided controls
		//Set analogs based on analogs
		pad_data->Lx += getAnalog(controllerConfig.ana_analog_right);
		pad_data->Lx -= getAnalog(controllerConfig.ana_analog_left);
		pad_data->Ly += getAnalog(controllerConfig.ana_analog_down);
		pad_data->Ly -= getAnalog(controllerConfig.ana_analog_up);
		
		//set analog based on digitals, overrides analogs!
		if ((controllerConfig.analog_up.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.analog_up.turbo & dupe_pad.Buttons)))
			pad_data->Ly = 0;
		if ((controllerConfig.analog_right.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.analog_right.turbo & dupe_pad.Buttons)))
			pad_data->Lx = 255;
		if ((controllerConfig.analog_down.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.analog_down.turbo & dupe_pad.Buttons)))
			pad_data->Ly = 255;
		if ((controllerConfig.analog_left.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.analog_left.turbo & dupe_pad.Buttons)))
			pad_data->Lx = 0;
		
		//Digitals
		pad_data->Buttons |= ((controllerConfig.digital_U.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.digital_U.turbo & dupe_pad.Buttons)))?PSP_CTRL_UP:0;
		pad_data->Buttons |= ((controllerConfig.digital_R.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.digital_R.turbo & dupe_pad.Buttons)))?PSP_CTRL_RIGHT:0;
		pad_data->Buttons |= ((controllerConfig.digital_D.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.digital_D.turbo & dupe_pad.Buttons)))?PSP_CTRL_DOWN:0;
		pad_data->Buttons |= ((controllerConfig.digital_L.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.digital_L.turbo & dupe_pad.Buttons)))?PSP_CTRL_LEFT:0;

		//Right side buttons
		pad_data->Buttons |= ((controllerConfig.button_Tri.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.button_Tri.turbo & dupe_pad.Buttons)))?PSP_CTRL_TRIANGLE:0;
		pad_data->Buttons |= ((controllerConfig.button_O.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.button_O.turbo & dupe_pad.Buttons)))?PSP_CTRL_CIRCLE:0;
		pad_data->Buttons |= ((controllerConfig.button_X.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.button_X.turbo & dupe_pad.Buttons)))?PSP_CTRL_CROSS:0;
		pad_data->Buttons |= ((controllerConfig.button_Sqr.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.button_Sqr.turbo & dupe_pad.Buttons)))?PSP_CTRL_SQUARE:0;
		
		//Start/Select
		pad_data->Buttons |= ((controllerConfig.button_Start.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.button_Start.turbo & dupe_pad.Buttons)))?PSP_CTRL_START:0;
		pad_data->Buttons |= ((controllerConfig.button_Select.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.button_Select.turbo & dupe_pad.Buttons)))?PSP_CTRL_SELECT:0;
		
		//Shoulders
		pad_data->Buttons |= ((controllerConfig.trigger_L.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.trigger_L.turbo & dupe_pad.Buttons)))?PSP_CTRL_LTRIGGER:0;
		pad_data->Buttons |= ((controllerConfig.trigger_R.pressed & dupe_pad.Buttons) || (inTurbo && (controllerConfig.trigger_R.turbo & dupe_pad.Buttons)))?PSP_CTRL_RTRIGGER:0;
		
		#ifdef ENABLE_WIFI
		pad_data->Buttons |= wifi_override;
		#endif
		
		if (pad_data->Buttons) //if there are any keys set, send a power stay on!
			scePowerTick(0);
		
		//Check for disabling
		if (!holdingNote && (dupe_pad.Buttons & PSP_CTRL_NOTE))
		{
			printf("Disabling\n");
			enabled = 0;
			holdingNote = 1;
			sprintf(menuPrintText, "Disabled");
			runMenu = MENU_PRINT_TEXT;
		}
	}
	else
	{
		if (!holdingNote && (dupe_pad.Buttons & PSP_CTRL_NOTE))
		{
			printf("Enabling\n");
			holdingNote = 1;
			enabled = 1;
			sprintf(menuPrintText, "Enabled");
			runMenu = MENU_PRINT_TEXT;
		}
	}
}

int sceCtrlPeekBufferPositiveFake(SceCtrlData *pad_data, int count)
{
	u32 k1;
	k1 = pspSdkSetK1(0);
	int res = ((FUNC_CTRL) mainHookSave[0].modfunc.addr)(pad_data, count);
	
	overrideControls(pad_data);
	pspSdkSetK1(k1);

	return res;
}

int sceCtrlPeekBufferNegativeFake(SceCtrlData *pad_data, int count)
{
	u32 k1;
	k1 = pspSdkSetK1(0);
	int res = ((FUNC_CTRL) mainHookSave[1].modfunc.addr)(pad_data, count);
	
	overrideControls(pad_data);
	pspSdkSetK1(k1);
	
	return res;
}

int sceCtrlReadBufferPositiveFake(SceCtrlData *pad_data, int count)
{
	u32 k1;
	k1 = pspSdkSetK1(0);
	int res = ((FUNC_CTRL) mainHookSave[2].modfunc.addr)(pad_data, count);

	overrideControls(pad_data);
	pspSdkSetK1(k1);
	
	return res;
}

int sceCtrlReadBufferNegativeFake(SceCtrlData *pad_data, int count)
{
	u32 k1;
	k1 = pspSdkSetK1(0);
	int res = ((FUNC_CTRL) mainHookSave[3].modfunc.addr)(pad_data, count);
	
	overrideControls(pad_data);
	pspSdkSetK1(k1);
	
	return res;
}

unsigned int previousPressed;

int sceCtrlPeekLatchFake(SceCtrlLatch *latch_data)
{
	SceCtrlData pad;
	int res = sceCtrlPeekBufferPositiveFake(&pad, 1);
	
	//Gen new Latch
	latch_data->uiMake  = (previousPressed ^ pad.Buttons) & pad.Buttons;
	latch_data->uiBreak = (previousPressed ^ pad.Buttons) & previousPressed;
	
	latch_data->uiPress   =  pad.Buttons;
	latch_data->uiRelease = ~pad.Buttons;
	
	previousPressed = pad.Buttons;
	return res;
}

int sceCtrlReadLatchFake(SceCtrlLatch *latch_data)
{
	SceCtrlData pad;
	int res = sceCtrlPeekBufferPositiveFake(&pad, 1);
	//For one reason or another this is no good. Makes games run too slow
	//int res = sceCtrlReadBufferPositiveFake(&pad, 1);
	
	//Gen new Latch
	latch_data->uiMake  = (previousPressed ^ pad.Buttons) & pad.Buttons;
	latch_data->uiBreak = (previousPressed ^ pad.Buttons) & previousPressed;
	
	latch_data->uiPress   =  pad.Buttons;
	latch_data->uiRelease = ~pad.Buttons;
	
	previousPressed = pad.Buttons;
	return res;
}

int mainThread (SceSize args, void *argp)
{
	//SceUID modid;
	SceUID loaderid, fd;
	int ret, x;
	
	fd = 0; //shutup about unused ;)
	previousPressed = 0;
	
#ifndef BUILD_DEVHOOK	///Disable these for devhook
	// Get thread id of bootstrap loader
	loaderid = strtoul((char *) argp,NULL,16);
	// Unload bootstrap loader
	sceKernelWaitThreadEnd(loaderid,NULL);
	moduleUnload(MAIN_LOADER_NAME);
#endif
	
#ifdef ENABLE_WIFI
	initwifi();
#endif

	//Make sure kernel is in analog sampling mode!
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	
	// Patch syscall table, here the program use jmp func in stub table, so patch not applied here but in all user modules
	for (x=0;x<MAIN_HOOK_NBR;x++)
	{
		ret = moduleHookFunc(&mainHookSave[x].modfunc, sceKernelSearchModuleByName(mainHookSave[x].modname), mainHookSave[x].libname, mainHookSave[x].nid, mainHookSave[x].func);
	
		HOOK_LOGFILE_WRITE("moduleHookFunc[%d] return : 0x%X\r\n-> modname : %s\r\n-> libname : %s\r\n-> nid : 0x%X\r\n\r\n",x,ret,mainHookSave[x].modname,mainHookSave[x].libname,mainHookSave[x].nid);
	}

//	readMacro("host1:/remaps/macros/tek_asuka.txt");
//	controllerConfig = readConfig("host1:/defaults.txt");//Load the config
//	controllerConfig = readConfig("host1:/remaps/swap_digi_ana.txt");//Load the config

#ifndef BUILD_DEVHOOK
 	run_menu();
#endif

#ifdef LOADING_UMD			///UMD LOADING CODE
	u32 modid;
	// If UMD present
	if (sceUmdCheckMedium(0))
	{
		// Mount UMD to disc1: file system
		sceUmdActivate(1,"disc0:");
		
		// Wait for init
		sceUmdWaitDriveStat(UMD_WAITFORINIT);
	}
	
	// Find umd id string
	fd = sceIoOpen(MAIN_UMDFILE_ID,PSP_O_RDONLY,0);
	if (fd >= 0)
	{
		// Read id
		sceIoRead(fd,mainUmdId,10);
		
		// End string
		mainUmdId[10] = 0;
		
		// Close file
		sceIoClose(fd);
	}
	else
		strcpy(mainUmdId,"unknow");
	
	// Debug line
	printf("ID: %s\n", mainUmdId);
	
	// Load game
	modid = moduleLoad(MAIN_UMDFILE_EXEC,0,1);
	
	// Start module
	sceKernelStartModule(modid,0,NULL,NULL,NULL);
#endif
#ifdef LOADING_HB			///HB LOADING
	// Load game
	//modid = moduleLoad("host1:/speed.elf",0,1);
	modid = moduleLoad("host1:/aa.elf",0,1);
	//modid = moduleLoad("ms0:/out.elf",0,1);
	
	sceKernelStartModule(modid,0,NULL,NULL,NULL);
#endif //LOADING_HB
#ifdef BUILD_MENU
	launchMenu();
#endif //BUILD_MENU
 
#ifdef ENABLE_WIFI
	udpWait();
#endif //ENABLE_WIFI

#ifdef BUILD_DEVHOOK
	runMenu = MENU_AUTO_UMD;
#endif
	// Exit thread
	sceKernelExitDeleteThread(0);
	
	return 0;
}

// Entry point

int module_start (SceSize args, void *argp)
{
	SceUID thid;
	
	// Start debug
	HOOK_LOGFILE_INIT;
	

	thid = sceKernelCreateThread("RemapspMain",mainThread,8,0x10000,0,NULL);
	if (thid >= 0) sceKernelStartThread(thid,args,argp);
	
	thid = sceKernelCreateThread("RemapspMenu",menuThread,8,0x10000,0,NULL);
	if (thid >= 0) sceKernelStartThread(thid,args,argp);
	
	return 0;
}

int module_stop (void)
{
	int x;
	// Restore functions
	for (x = 0; x < MAIN_HOOK_NBR; x++)
		moduleRestoreFunc(&mainHookSave[x].modfunc);

	return 0;
}

