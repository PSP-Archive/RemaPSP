#include "main.h"

#ifdef BUILD_MENU

#include "blit.h"
#include "multiselect.h"
#define path "ms0:/PSP/GAME"

void launchMenu()
{
	char** options;
	SceIoDirent dir;
	int dfd, ok;
	SceUID modid;
	
	int optionsSize = 0;
	int selected = 0;
	
//	pspDebugScreenClear();
	blit_clearscreen();
	
	
	//Grab 15*50 (char) + 10*(char*)
	SceUID mem = sceKernelAllocPartitionMemory(1, "block", 0, (15*50*sizeof(char))+(15*sizeof(char*)), NULL);
	void *buf = sceKernelGetBlockHeadAddr(mem);
	
	options = buf;
	for (dfd = 0; dfd < 15; dfd++)
	{
		options[dfd] = buf + (15*sizeof(char*)) + (dfd*50*sizeof(char));
	}
	
	dfd = sceIoDopen(path);
	if (dfd>=0)
	{
		for (;;)
		{
			memset(&dir, 0, sizeof dir);
			ok = sceIoDread(dfd, &dir);
			if (ok <= 0 || optionsSize >= 10) break;
			//extract info from dir
			if (FIO_SO_ISDIR(dir.d_stat.st_attr))
			{
				if (dir.d_name[0] != '%' && dir.d_name[strlen(dir.d_name)-1] != '%' && dir.d_name[0] != '.') //skip %/. dirs
				{
					sprintf(options[optionsSize], "%s/%s/EBOOT.PBP", path, dir.d_name);
					printf("%i - %s\n", optionsSize, options[optionsSize]);
					optionsSize++;
				}
				else
				{
					printf("Skipped %s\n", dir.d_name);
				}
			}
			else
			{
				printf("not dir %s\n", dir.d_name);
			}
		}
		sceIoDclose(dfd);
	}
	else
	{
		printf("Unable to open psp/game?\n");
	}

	do
	{
		selected = multiselect(options, optionsSize, 0, "Choose a homebrew to launch (RemaPSP by danzel)");
	}
	while (selected == -1);
	
	
	
/*	struct SceKernelLoadExecParam ebootLauncher;
	
	ebootLauncher.args = strlen(options[selected])+1;
	ebootLauncher.argp = options[selected];
	ebootLauncher.key = NULL;
	ebootLauncher.size = sizeof(ebootLauncher) + ebootLauncher.args;
	if((modid = sceKernelLoadExec(options[selected],&ebootLauncher))<0)
	{
		printf("FAILED: %i\n", modid);
	}
	else
	{
		printf("RUNNING\n");
	}
*/	
	
	// Load game
	//modid = moduleLoad("host1:/speed.elf",0,1);
	modid = moduleLoad(options[selected],0,1);
	printf("moduleLoad(%s); == %i\n", options[selected], modid);
	//modid = moduleLoad("ms0:/out.elf",0,1);
	modid = sceKernelStartModule(modid,0,NULL,NULL,NULL);
	printf("startmod = %i\n", modid);
	
	sceKernelFreePartitionMemory(mem);

}
#endif
