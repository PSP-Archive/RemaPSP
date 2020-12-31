	.set noreorder

#include "pspstub.s"
	STUB_START "NKLIB",0x40010000,0x00020005
	STUB_FUNC  0x977C76F2,nkThreadSuspend
	STUB_FUNC  0x0E3BBEE8,nkThreadResume
	STUB_END
