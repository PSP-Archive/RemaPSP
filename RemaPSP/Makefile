TARGET = remapsp
OBJS = main.o Utils/module.o exports.o conf.o wifi.o menu.o multiselect.o blit.o launchmenu.o NKLIB.o

# Define to build this as a prx (instead of a static elf)
BUILD_PRX = 1

# Define the name of our custom exports (minus the .exp extension)
PRX_EXPORTS=exports.exp

#USE_PSPSDK_LIBC = 1

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

INCDIR = 
#CFLAGS = -O2 -G0 -Wall
#CFLAGS = -O2 -G0 -Wall -fno-builtin-printf
CFLAGS = -Os -G0 -Wall -fno-strict-aliasing -fno-builtin-printf
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = -mno-crt0 -nostartfiles
#LIBS = -lpspumd -lpspsdk -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver
#LIBS = -lpspumd
#LIBS = -lpsphprm -lpsppower -lpspsdk -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpspge -lpsprtc
LIBS = -lpsphprm -lpsppower -lpspsdk -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpspge -lpsprtc -lpspumd
#-lpspdebug 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
