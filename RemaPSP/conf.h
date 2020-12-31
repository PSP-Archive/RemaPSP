#ifndef INCLUDED_CONF_H
#define INCLUDED_CONF_H

#include <pspctrl.h>
#include <psphprm.h>
#include <pspkernel.h>

struct _button
{
	u32 pressed;	//+
	u32 turbo;		//~
	//Unpressed
	//Unturbo? lol
};
typedef struct _button button;

struct ctrlSetup
{
	//Digital directions
	button digital_U;
	button digital_R;
	button digital_D;
	button digital_L;
	
	//Buttons on the right ;)
	button button_Tri;
	button button_O;
	button button_X;
	button button_Sqr;
	
	button button_Start;
	button button_Select;
	
	button trigger_L;
	button trigger_R;

	//Analogs -- TODO: make these u32's for some speed up?
	button analog_up;    u16 ana_analog_up;
	button analog_right; u16 ana_analog_right;
	button analog_down;  u16 ana_analog_down;
	button analog_left;  u16 ana_analog_left;
	//Analogs are 'special'
	//their .pressed are for buttons to push them in that way fully
	//their .turbo are for buttons to push them in that way 50%
	//their u16 are for analog sticks to push them in that direction (analog)

	//Digital presses override analog, more than one analog input into any analog will mess it up.
};
//Some extra defines for the space we are using for analog directions.
#define CTRL_ANALOG_UP    0x0002
#define CTRL_ANALOG_RIGHT 0x0004
#define CTRL_ANALOG_DOWN  0x0400
#define CTRL_ANALOG_LEFT  0x0800

struct ctrlSetup readConfig(char* configName);




//And Macro bits:
#define MACRO_MAX_SIZE 40
typedef struct _macroHolder
{
	button trigger;
	
	u32 size;
	
	unsigned char runYet; //if 1 then only run once (and set to 0) if 2 then run whenever pressed
	
	button buttons[MACRO_MAX_SIZE];
	u32 time[MACRO_MAX_SIZE];
} macroHolder;

extern macroHolder Macro[2]; //seriously. the only one... okay 2 :P :P



#endif //INCLUDED_CONF_H
