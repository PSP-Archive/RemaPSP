TARGET = remapspboot
OBJS = bootstrap.o exports.o

USE_PSPSDK_LIBC = 1

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = -mno-crt0 -nostartfiles

LIBS =

PSP_EBOOT_ICON = icon0.png
PSP_EBOOT_PIC1 = pic1.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
