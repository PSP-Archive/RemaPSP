#ifndef INCLUDED_BLIT_H
#define INCLUDED_BLIT_H

//blit.c contains code from the vshex from devhook
//The included font is from the pspsdk
int blit_string(int sx,int sy,const char *msg,int fg_col,int bg_col);

void blit_clearscreen();

#endif //INCLUDED_BLIT_H
