#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT 14567

enum PspCtrlButtons
{
	/** Select button. */
	PSP_CTRL_SELECT     = 0x000001,
	/** Start button. */
	PSP_CTRL_START      = 0x000008,
	/** Up D-Pad button. */
	PSP_CTRL_UP         = 0x000010,
	/** Right D-Pad button. */
	PSP_CTRL_RIGHT      = 0x000020,
	/** Down D-Pad button. */
	PSP_CTRL_DOWN      	= 0x000040,
	/** Left D-Pad button. */
	PSP_CTRL_LEFT      	= 0x000080,
	/** Left trigger. */
	PSP_CTRL_LTRIGGER   = 0x000100,
	/** Right trigger. */
	PSP_CTRL_RTRIGGER   = 0x000200,
	/** Triangle button. */
	PSP_CTRL_TRIANGLE   = 0x001000,
	/** Circle button. */
	PSP_CTRL_CIRCLE     = 0x002000,
	/** Cross button. */
	PSP_CTRL_CROSS      = 0x004000,
	/** Square button. */
	PSP_CTRL_SQUARE     = 0x008000,
	/** Home button. */
	PSP_CTRL_HOME       = 0x010000,
	/** Hold button. */
	PSP_CTRL_HOLD       = 0x020000,
	/** Music Note button. */
	PSP_CTRL_NOTE       = 0x800000,
};

int main()
{
	int udpSendSock;
	int err;
	udpSendSock = socket(PF_INET, SOCK_DGRAM, 0);
	
	struct sockaddr_in udpsock_addr;
	udpsock_addr.sin_family = AF_INET;
	udpsock_addr.sin_port = htons(PORT);
	inet_aton("10.15.10.12", &udpsock_addr.sin_addr);     //TODO REPLACE
//	inet_aton("127.0.0.1", &udpsock_addr.sin_addr);     //TODO REPLACE
	err = connect(udpSendSock, (struct sockaddr*)&udpsock_addr, sizeof(udpsock_addr));

	char payload[10];
	unsigned int but = 0;
	memcpy(payload, &but, 4);

	err = sendto(udpSendSock, payload, 4, /*FLAGS*/0, (struct 
sockaddr*)&udpsock_addr, sizeof(udpsock_addr));

	return 0;
}
