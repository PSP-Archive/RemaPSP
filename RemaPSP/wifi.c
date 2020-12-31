#include "main.h"

#ifdef ENABLE_WIFI
#include <pspsdk.h>
#include <pspnet_apctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 14567

u32 wifi_override = 0;
unsigned char wifion = 0;

//HACK LOWEL
int snprintf(char *str, size_t size, const char *format, ...)
{	return 0;	}

/* Connect to an access point */
int connectToApctl(int config)
{
	int err;
	int stateLast = -1;
	
	if((err = pspSdkInetInit()))
	{
		printf("ERROR: could not initialise the network %08X\n", err);
		return 0;
	}
	
connectWifi:
	/* Connect using the first profile */
	err = sceNetApctlConnect(config);
	if (err != 0)
	{
		printf("ERROR: sceNetApctlConnect returns %08X\n", err);
		return 0;
	}

	printf("Connecting...\r\n");
	while (1)
	{
		int state;
		err = sceNetApctlGetState(&state);
		if (err != 0)
		{
			printf("ERROR: sceNetApctlGetState returns %x\r\n", err);
			break;
		}
		if (state != stateLast)
		{
			if (stateLast == 2 && state == 0)
			{
				printf("  Connecting to wifi Failed, Retrying...\r\n");
				sceKernelDelayThread(500*1000); // 500ms
				goto connectWifi;
			}
			printf("  connection state %d of 4\r\n", state);
			stateLast = state;
		}
		if (state == 4)
			break;  // connected with static IP

		// wait a little before polling again
		sceKernelDelayThread(50*1000); // 50ms
	}
	printf("Connected!\n");
	return 1;
}

//Simple udp listener.
//Prints packets to printf and sets the wifi_override if size==4
void udpWait()
{
	int sock;

	char buffer[5];
	
	sock = sceNetInetSocket(PF_INET, SOCK_DGRAM, 0);
	
	struct sockaddr_in updserv_addr;
	updserv_addr.sin_family = AF_INET;
	updserv_addr.sin_port = htons(PORT);
	updserv_addr.sin_addr.s_addr = INADDR_ANY;

	int err = sceNetInetBind(sock, (struct sockaddr*)&updserv_addr, sizeof(updserv_addr));
	printf("Bind result: %i\nerrno:%i\n", err, sceNetInetGetErrno());
	HOOK_LOGFILE_WRITE("Bind result: %i\nerrno:%i\n", err, sceNetInetGetErrno());
	
	while (1)
	{
		ssize_t length = sceNetInetRecvfrom(sock, buffer, 4, 0, 0, 0);
		if (length == 4)
			wifi_override = *((int*)buffer);
		printf("length = %i\nData: %x\n", length, *((int*)buffer));
	}
	close(sock);
}

int XpspSdkLoadInetModules()
{
	int modID;

	HOOK_LOGFILE_WRITE("flash0:/kd/ifhandle.prx\n");
	modID = pspSdkLoadStartModule("flash0:/kd/ifhandle.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (modID < 0)
		return modID;

	HOOK_LOGFILE_WRITE("flash0:/kd/pspnet.prx\n");
	modID = pspSdkLoadStartModule("flash0:/kd/pspnet.prx", PSP_MEMORY_PARTITION_USER); 
	if (modID < 0)
	{
		HOOK_LOGFILE_WRITE("Failed: %08X\n", modID);
//		return modID;
	}
	else
	{
		HOOK_LOGFILE_WRITE("fixup\n");
		pspSdkFixupImports(modID);
	}
	
	HOOK_LOGFILE_WRITE("flash0:/kd/pspnet_inet.prx\n");
	modID = pspSdkLoadStartModule("flash0:/kd/pspnet_inet.prx", PSP_MEMORY_PARTITION_USER); 
	if (modID < 0)
	{
		HOOK_LOGFILE_WRITE("Failed: %08X\n", modID);
//		return modID;
	}
	else
	{
		HOOK_LOGFILE_WRITE("fixup\n");
		pspSdkFixupImports(modID);
	}

	HOOK_LOGFILE_WRITE("flash0:/kd/pspnet_apctl.prx\n");
	modID = pspSdkLoadStartModule("flash0:/kd/pspnet_apctl.prx", PSP_MEMORY_PARTITION_USER); 
	if (modID < 0)
	{
		HOOK_LOGFILE_WRITE("Failed: %08X\n", modID);
//		return modID;
	}
	else
	{
		HOOK_LOGFILE_WRITE("fixup\n");
		pspSdkFixupImports(modID);
	}
/*
	modID = loadModule("flash0:/kd/pspnet_ap_dialog_dummy.prx", PSP_MEMORY_PARTITION_USER); 
	if (modID < 0)
		return modID;
	else
		fixupImports(modID);
*/
	HOOK_LOGFILE_WRITE("flash0:/kd/pspnet_resolver.prx\n");
	modID = pspSdkLoadStartModule("flash0:/kd/pspnet_resolver.prx", PSP_MEMORY_PARTITION_USER); 
	if (modID < 0)
	{
		HOOK_LOGFILE_WRITE("Failed: %08X\n", modID);
//		return modID;
	}
	else
	{
		HOOK_LOGFILE_WRITE("fixup\n");
		pspSdkFixupImports(modID);
	}

	return 0;
}

void initwifi()
{
	printf("initwifi()\n");
	int err;
	if ((err = XpspSdkLoadInetModules()))
	{
		printf( ": Error, could not load modules %08X\n", err);
	}
	HOOK_LOGFILE_WRITE("Load Modules (%08X) want 0\n", err);
	err = connectToApctl(1);
	printf( "connectToApctl returns %i (want 1)\n", err);
	HOOK_LOGFILE_WRITE("connectToApctl (%i) want 1\n", err);
}
#endif //ENABLE_WIFI
