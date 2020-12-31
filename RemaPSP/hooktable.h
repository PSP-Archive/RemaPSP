
#ifndef HOOKTABLE_INCLUDED
#define HOOKTABLE_INCLUDED


#define	MAIN_HOOK_NBR	6

MainHook mainHookSave[MAIN_HOOK_NBR] =
{
	{ { 0, NULL }, "sceController_Service", "sceCtrl", 0x3A622550, sceCtrlPeekBufferPositiveFake },
	{ { 0, NULL }, "sceController_Service", "sceCtrl", 0xC152080A, sceCtrlPeekBufferNegativeFake },
	{ { 0, NULL }, "sceController_Service", "sceCtrl", 0x1F803938, sceCtrlReadBufferPositiveFake },
	{ { 0, NULL }, "sceController_Service", "sceCtrl", 0x60B81F86, sceCtrlReadBufferNegativeFake },

	{ { 0, NULL }, "sceController_Service", "sceCtrl", 0xb1d0e5cd, sceCtrlPeekLatchFake },
	{ { 0, NULL }, "sceController_Service", "sceCtrl", 0x0b588501, sceCtrlReadLatchFake },


//	Attempt at getting nid overloading working with menus.
//	Can't get this to work: moduleHookFunc[4] return : 0x2
//  Incorrect NID or something? :(
//	{ { 0, NULL }, "sceVshBridge_Driver", "sceVshBridge", 0xc6395c03, vshCtrlReadBufferPositiveFake },
};

#endif
