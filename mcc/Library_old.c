#include <exec/libraries.h>
#include <exec/memory.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <stormamiga.h>

extern "C"
{
	ULONG MCC_Query (register __d0 LONG);
	VOID kprintf (STRPTR, ... );
	VOID abortLibInit ();
	VOID ioinit_ ();
}

BOOL Init (struct Library *base);
VOID Cleanup ();
extern struct MUI_CustomClass *HTMLviewClass;

struct LibBase
{
	struct Library base;
};
#pragma libbase LibBase

UBYTE LibVersionString[] = "$VER: HTMLview.mcc 12.6 (" __DATE2__ ") © 1998-2000 Allan Odgaard";

VOID _INIT_7_InitLib (register __a6 struct Library *base)
{
	BOOL succes = FALSE;

#ifdef TIME_LIMIT
	struct DateStamp ds;
	struct EasyStruct message =
	{
		sizeof(struct EasyStruct), 0, "HTMLview.mcc",
		"This beta version is now\n"
		"more than three months old!\n"
		"Please get a fresh one from\n"
		"http://www.diku.dk/students/duff/",
		"OK"
	};

	DateStamp(&ds);
	if(ds.ds_Days > 7918+3*30)
		EasyRequest(NULL, &message, NULL, NULL);
#endif

	UBYTE *newstack;
	if(newstack = new UBYTE [2000])
	{
		UBYTE *upper = newstack+2000;
		struct StackSwapStruct stackswap;
		stackswap.stk_Lower		= newstack;
		stackswap.stk_Upper		= (ULONG)upper;
		stackswap.stk_Pointer	= upper;
		StackSwap(&stackswap);

		succes = Init(base);

		StackSwap(&stackswap);
		delete newstack;
	}

	if(!succes || !DataTypesBase)
		abortLibInit();
}

VOID _EXIT_7_ExpungeLib (VOID)
{
	Cleanup();
}

ULONG	MCC_Query (register __d0 LONG which)
{
	return(which ? NULL : (ULONG)HTMLviewClass);
}
