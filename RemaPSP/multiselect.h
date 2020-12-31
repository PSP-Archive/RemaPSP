#ifndef INCLUDED_DANZELS_MULTISELECT
#define INCLUDED_DANZELS_MULTISELECT

//Made by danzel :)
//You can change the buttons by editting the top of multiselect.c
//defaults are X: select,    /\: Cancel 
/** Simple function for a menu, pass it an array of choices and it will print em and allow selection.
 *  The index of the selected element will be returned at the end, or -1 if cancel is pressed.
 *  @param picks     - an array of strings which are the possible choices to select from
 *  @param count     - The amount of elements in the picks array
 *  @param selected  - The index of the initially selected item
 *  @param nessage   - The message printed out before the selection
 */
int multiselect(char* picks[], int count, int selected, char* message);

#endif
