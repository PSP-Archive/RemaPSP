#ifndef INCLUDED_MENU_H
#define INCLUDED_MENU_H


extern char menuPrintText[100];
extern unsigned char runMenu;
//Possible values for runMenu
#define MENU_DO_NOTHING 0
#define MENU_RUN_MENU 1
#define MENU_PRINT_TEXT 2
//Auto load the ID from the UMD and try autoload it
#define MENU_AUTO_UMD 3

int menuThread (SceSize args, void *argp);

void run_menu();

#endif //INCLUDED_MENU_H
