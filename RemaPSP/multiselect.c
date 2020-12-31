//Code by danzel, give credit where credit is due :)
#include <pspdebug.h>
#include <pspctrl.h>
#include "blit.h"
#include <stdio.h>

#define SELECTBUTTON PSP_CTRL_CROSS
#define CANCELBUTTON PSP_CTRL_TRIANGLE

/** Simple function for a menu, pass it an array of choices and it will print em and allow selection.
 *  The index of the selected element will be returned at the end, or -1 if cancel is pressed.
 *  @param  picks     - an array of strings which are the possible choices to select from
 *  @param  count     - The amount of elements in the picks array
 *  @param  selected  - The index of the initially selected item
 *  @param  message   - The message printed out before the selection
 *  @return The chosen item, or -1 if cancel is pressed.
 */
int multiselect(char* picks[], int count, int selected, char* message)
{
//	pspDebugScreenClear();
	
	int done = 0;
	while (!done)
	{
		SceCtrlData pad;
		int onepressed = 0;
		int loop;
		
//		pspDebugScreenSetXY(0,0);
		
		do
		{
			blit_string(1, 0, message, 0xffffffff, 0);
			
			for (loop = 0; loop < count; loop++)
			{
				if (selected == loop)
				{
					blit_string(1, (loop+3)*9, "> ", 0xffffffff, 0);
				}
				else
				{
					blit_string(1, (loop+3)*9, "  ", 0xffffffff, 0);
				}
				blit_string(3, (loop+3)*9, picks[loop], 0xffffffff, 0);
			}
				
			/*now loop on input, let it fall through to redraw, if it is X then break*/
			sceCtrlReadBufferPositive(&pad, 1); 
			onepressed = (  (pad.Buttons & SELECTBUTTON ) ||
							(pad.Buttons & CANCELBUTTON ) ||
							(pad.Buttons & PSP_CTRL_UP  ) ||
							(pad.Buttons & PSP_CTRL_DOWN));
		}
		while (!onepressed);/*While havent pressed a key*/
		/*Find the key and change based on it*/
		if (pad.Buttons & SELECTBUTTON) done = 1;
		if (pad.Buttons & PSP_CTRL_UP) selected = (selected + count - 1) % count;
		if (pad.Buttons & PSP_CTRL_DOWN) selected = (selected+1) % count;
		if (pad.Buttons & CANCELBUTTON) { done = 1; selected = -1; }
		while (onepressed)/*Wait for Key Release*/
		{
			sceCtrlReadBufferPositive(&pad, 1); 
			onepressed = (  (pad.Buttons & SELECTBUTTON ) ||
							(pad.Buttons & CANCELBUTTON ) ||
							(pad.Buttons & PSP_CTRL_UP  ) ||
							(pad.Buttons & PSP_CTRL_DOWN));
		}
	}
	return selected;
}
