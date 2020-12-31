#ifndef INCLUDED_WIFI_H
#define INCLUDED_WIFI_H

#include "main.h"
#ifdef ENABLE_WIFI

extern u32 wifi_override;

void initwifi();
void udpWait();

#endif //ENABLE_WIFI

#endif //INCLUDED_WIFI_H
