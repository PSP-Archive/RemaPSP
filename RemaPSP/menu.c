#include <pspiofilemgr.h>
#include <string.h>
#include <stdio.h>
//#include <pspdebug.h>

#include "main.h"
#include "menu.h"
#include "blit.h"
#include "multiselect.h"

//From NKThread
int nkThreadSuspend(SceUID thId);
int nkThreadResume(SceUID thId);


char menuPrintText[100];
unsigned char runMenu = MENU_DO_NOTHING;

#if defined NODEBUG
const char* path = "ms0:/remaps";
#else
const char* path = "host1:/remaps";
#endif

/*
 * Thread that waits around untill runMenu is 1, then runs the menu, setting it back to zero when done!
 */
int menuThread (SceSize args, void *argp)
{
	while (1)
	{
		int a = 0; //for use in the bottom of the switch
		switch (runMenu)
		{
		case MENU_DO_NOTHING:
			sceKernelDelayThread(1000*200);
			break;
		
		case MENU_RUN_MENU:
			printf("getting Id\n");
			int ourThreadId = sceKernelGetThreadId();
			printf("ID: %i\n", ourThreadId);
			
			printf("Suspending\n");
			nkThreadSuspend(ourThreadId); ///BEGIN LOCK
				printf("Suspended\n");
	
			
				run_menu();
				runMenu = MENU_DO_NOTHING;
	
			nkThreadResume(ourThreadId);  ///END LOCK
			printf("Resumed\n");
			break;
		
		case MENU_PRINT_TEXT:
			for (; a < 1000 && runMenu==MENU_PRINT_TEXT; a++) //2 seconds at 50d0* a second?
			{
				blit_string(0,0,menuPrintText,0xffffffff,0);
				sceKernelDelayThread(5000);
			}
			runMenu = MENU_DO_NOTHING;
			break;
		
		//Auto load the ID from the UMD and try autoload it
		case MENU_AUTO_UMD:
			while (runMenu == MENU_AUTO_UMD)
			{
				sceKernelDelayThread(1000*1000);
				// Find umd id string
				a = sceIoOpen(MAIN_UMDFILE_ID,PSP_O_RDONLY,0);
				if (a >= 0)
				{
					char UMDId[11];
					// Read id
					sceIoRead(a,UMDId,10);
					
					// End string
					UMDId[10] = 0;
					
					// Close file
					sceIoClose(a);
					
					//Now try open its corresponding blah blah
					//TODO
					sprintf(menuPrintText, "ms0:/remaps/auto/%s.txt", UMDId);
					a = sceIoOpen(menuPrintText, PSP_O_RDONLY, 0);
					if (a >= 0) //Managed to load the config, it exists
					{
						sceIoClose(a);
						controllerConfig = readConfig(menuPrintText);
						enabled = 1;
					}
					runMenu = MENU_DO_NOTHING;
				}
			}
			break;
		}
	}
	return 0;
}

#define MAX_MENU_SIZE 15

void run_menu()
{
	char** options;
	SceIoDirent dir;
	int dfd, ok;
	
	int optionsSize = 0;
	int selected = 0;
	
	
	//Grab 10*50 (char) + 10*(char*)
	SceUID mem = sceKernelAllocPartitionMemory(1, "block", 0, (MAX_MENU_SIZE*50*sizeof(char))+(MAX_MENU_SIZE*sizeof(char*)), NULL);
	void *buf = sceKernelGetBlockHeadAddr(mem);
	
	options = buf;
	for (dfd = 0; dfd < MAX_MENU_SIZE; dfd++)
	{
		options[dfd] = buf + (MAX_MENU_SIZE*sizeof(char*)) + (dfd*50*sizeof(char));
	}
	
	dfd = sceIoDopen(path);
	if (dfd>=0)
	{
		for (;;)
		{
			memset(&dir, 0, sizeof dir);
			ok = sceIoDread(dfd, &dir);
			if (ok <= 0 || optionsSize >= MAX_MENU_SIZE) break;
			//extract info from dir
			if (FIO_SO_ISREG(dir.d_stat.st_attr))
			{
				sprintf(options[optionsSize], "%s/%s", path, dir.d_name);
				printf("%i - %s\n", optionsSize, options[optionsSize]);
				optionsSize++;
			}
		}
		sceIoDclose(dfd);
	}
	
//	pspDebugScreenClear();

	if (optionsSize == 0)
	{
		blit_string(1,1,"Can't find any remaps!",0xffffffff,0);
		sceKernelDelayThread(1000*1000*5);
		return;
	}
	
	selected = multiselect(options, optionsSize, 0, "Choose a new remap (RemaPSP by danzel)");
	
	if (selected == -1)
		return;
	
	controllerConfig = readConfig(options[selected]);//Load the config
	
	sceKernelFreePartitionMemory(mem);
}
