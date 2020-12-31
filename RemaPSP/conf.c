#include "main.h"
#include "conf.h"
#include <pspiofilemgr.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LINE_SIZE 1024


void readMacro(char* macroName, int macroNumber);

//Perform a mask operation :)
void mask(button *b, const char* line, const char* seq, const u32 val)
{
	char buffer[15];

	sprintf(buffer, "+%s", seq);
	if (strstr(line, buffer))
	{
		printf("%s ", buffer);
		b->pressed |= (val);
	}
	
	sprintf(buffer, "~%s", seq);
	if (strstr(line, buffer))
	{
		printf("%s ", buffer);
		b->turbo |= (val);
	}
}

//Add the standard button mask for all buttons and analogs
void getButtonMask(char* line, button* newBut)
{
	newBut->pressed = 0;
	newBut->turbo   = 0;
	
	mask(newBut, line, "d_up",    PSP_CTRL_UP);
	mask(newBut, line, "d_right", PSP_CTRL_RIGHT);
	mask(newBut, line, "d_down",  PSP_CTRL_DOWN);
	mask(newBut, line, "d_left",  PSP_CTRL_LEFT);

	mask(newBut, line, "triangle", PSP_CTRL_TRIANGLE);
	mask(newBut, line, "circle",   PSP_CTRL_CIRCLE);
	mask(newBut, line, "cross",    PSP_CTRL_CROSS);
	mask(newBut, line, "square",   PSP_CTRL_SQUARE);

	mask(newBut, line, "start",  PSP_CTRL_START);
	mask(newBut, line, "select", PSP_CTRL_SELECT);
	
	mask(newBut, line, "ltrigger", PSP_CTRL_LTRIGGER);
	mask(newBut, line, "rtrigger", PSP_CTRL_RTRIGGER);
	
	//NOTE ETC????
	
	//Luckily as the pspctrl stuff doesn't use the whole u32, we can use the top char to store these!
	mask(newBut, line, "r_left",  PSP_HPRM_BACK<<24);
	mask(newBut, line, "r_right", PSP_HPRM_FORWARD<<24);
	mask(newBut, line, "r_play",  PSP_HPRM_PLAYPAUSE<<24);
	mask(newBut, line, "r_up",    PSP_HPRM_VOL_UP<<24);
	mask(newBut, line, "r_down",  PSP_HPRM_VOL_DOWN<<24);
	
}

//Add the extra masks for the analog controls
//Should only be masked a maximum of once
void getAnalogMask(char* line, u16* ana)
{
	*ana = 0;
	
	if (strstr(line, "ana_up"))
	{
		printf("ana_up ");
		*ana = CTRL_ANALOG_UP;
	}
	if (strstr(line, "ana_right"))
	{
		printf("ana_right ");
		*ana = CTRL_ANALOG_RIGHT;
	}
	if (strstr(line, "ana_down"))
	{
		printf("ana_down ");
		*ana = CTRL_ANALOG_DOWN;
	}
	if (strstr(line, "ana_left"))
	{
		printf("ana_left ");
		*ana = CTRL_ANALOG_LEFT;
	}
}

//Add the extra controls we don't want controlling the analog controls
void getExtraDigital(char* line, button* newBut)
{
	mask(newBut, line, "ana_up",    CTRL_ANALOG_UP);
	mask(newBut, line, "ana_right", CTRL_ANALOG_RIGHT);
	mask(newBut, line, "ana_down",  CTRL_ANALOG_DOWN);
	mask(newBut, line, "ana_left",  CTRL_ANALOG_LEFT);
}

//Read a line from the file, throwing away comments.
//I hope this never crosses the 1024 boundary!
void readline(char* line, SceUID input)
{
	char in;
	int inComments = 0;
	int position = 0;
	unsigned int ret = 0;
	
	line[0] = '\0';
	
	while (1)
	{
		if (!(ret = sceIoRead(input, &in, 1)))
		{
//			printf("sceIoRead FAIL! return:%x\n", ret);
			line[position] = '\0';
			//Bah, failed to read.
			//Early end of file?
			return;
		}
		
		if (in == '#') inComments = 1;
		
		if (in == '\r') continue; //skip \r's
		if (in == '\n') break;
		
		if (!inComments)
		{
			line[position] = in;
			position++;
		}
	}
	line[position] = '\0';
}

void doChar(SceUID input, char* line, button* b)
{
	readline(line, input);
	getButtonMask(line, b);
	getExtraDigital(line, b);
	printf("\n");
}

void doAnalog(SceUID input, char* line, button* b, u16* ana)
{
	readline(line, input);
	getButtonMask(line, b);
	getAnalogMask(line, ana);
	printf("\n");
}

struct ctrlSetup readConfig(char* configName)
{
	printf("readConfig(%s)\n", configName);
	
	struct ctrlSetup ctrls;
	char line[MAX_LINE_SIZE];

	SceUID input = sceIoOpen(configName, PSP_O_RDONLY, 0777);
	printf("open() = %i\n", input);
	if (input == 0) //Failed to open!
	{
		printf("Couldn't open file!\n");
		return ctrls;
	}
	
	//Start reading em!
	readline(line, input); //Toss the first line
	
	printf("Digital Up: ");    doChar(input, line, &ctrls.digital_U);
	printf("Digital Right: "); doChar(input, line, &ctrls.digital_R);
	printf("Digital Down: ");  doChar(input, line, &ctrls.digital_D);
	printf("Digital Left: ");  doChar(input, line, &ctrls.digital_L);
	
	printf("Triangle: "); doChar(input, line, &ctrls.button_Tri);
	printf("Circle: ");   doChar(input, line, &ctrls.button_O);
	printf("Cross: ");    doChar(input, line, &ctrls.button_X);
	printf("Square: ");   doChar(input, line, &ctrls.button_Sqr);

	printf("Start: ");  doChar(input, line, &ctrls.button_Start);
	printf("Select: "); doChar(input, line, &ctrls.button_Select);
	
	printf("L Trigger: "); doChar(input, line, &ctrls.trigger_L);
	printf("R Trigger: "); doChar(input, line, &ctrls.trigger_R);
	
	printf("Y- Analog: "); doAnalog(input, line, &ctrls.analog_up,    &ctrls.ana_analog_up);
	printf("X+ Analog: "); doAnalog(input, line, &ctrls.analog_right, &ctrls.ana_analog_right);
	printf("Y+ Analog: "); doAnalog(input, line, &ctrls.analog_down,  &ctrls.ana_analog_down);
	printf("X- Analog: "); doAnalog(input, line, &ctrls.analog_left,  &ctrls.ana_analog_left);

	readline(line, input); //Read the first macro button
	printf("Macro: "); getButtonMask(line, &Macro[0].trigger); printf("\n");

	readline(line, input); //Read the first macro filename
	if (Macro[0].trigger.pressed != 0)
		readMacro(line, 0);

	readline(line, input); //Read the second macro button
	printf("Macro: "); getButtonMask(line, &Macro[1].trigger); printf("\n");

	readline(line, input); //Read the second macro filename
	if (Macro[1].trigger.pressed != 0)
		readMacro(line, 1);

	
	input = sceIoClose(input);
	printf("close() = %i\n", input);
	return ctrls;
}
//Macro bits:
macroHolder Macro[2]; //seriously. the only one... okay 2 :P :P

u32 getInt(char* line)
{
	u32 tmp = 0;
	int pos = 0;
	while (line[pos] >= '0' && line[pos] <= '9')
	{
		tmp *= 10;
		tmp += line[pos]-'0';
		pos++;
	}
	return tmp;
}

#if 0
typedef struct _macroHolder
{
	button trigger;
	
	u32 size;
	
	button buttons[MACRO_MAX_SIZE];
	u32 time[MACRO_MAX_SIZE];
} macroHolder;
#endif

void readMacro(char* macroName, int macroNumber)
{
	printf("readMacro(%s)\n", macroName);
	char line[MAX_LINE_SIZE];
#if defined NODEBUG
	sprintf(line, "ms0:/remaps/macros/%s", macroName);
#else
	sprintf(line, "host1:/remaps/macros/%s", macroName);
#endif

	
	SceUID input = sceIoOpen(line, PSP_O_RDONLY, 0777);
	printf("open() = %i\n", input);
	if (input == 0) //Failed to open!
	{
		printf("Couldn't open file!\n");
		return;
	}
	
	//Clear off the old macro
	Macro[macroNumber].size = 0;
	memset(&Macro[macroNumber].buttons, 0, sizeof(Macro[macroNumber].buttons));
	memset(&Macro[macroNumber].time, 0, sizeof(Macro[macroNumber].time));
	
	//Start reading em!
	readline(line, input); //Toss the first line
	
	//Get the size
	readline(line, input); Macro[macroNumber].size = getInt(line);
	printf("Size: %i\n", Macro[macroNumber].size);
	
	//Get if its a once only macro
	if (strstr(line, "once"))
	{
		printf("Once only\n");
		Macro[macroNumber].runYet = 1;
	}
	else
		Macro[macroNumber].runYet = 2;
	
	int a;
	for (a = 0; a < Macro[macroNumber].size; a++)
	{
		printf("Reading macro %i - ", a);
		readline(line, input);
		Macro[macroNumber].time[a] = getInt(line);
		printf("%i\n", Macro[macroNumber].time[a]);
		getButtonMask(line, &Macro[macroNumber].buttons[a]); printf("\n");
	}
	return;
}



/*SceUID mem = sceKernelAllocPartitionMemory(2, "block", 0, (3*pw + padding)*sizeof(unsigned char), NULL);
unsigned char *buf = (unsigned char*)sceKernelGetBlockHeadAddr(mem);
unsigned char buf[3*pw + padding];
*/
