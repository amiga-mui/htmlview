#ifndef ISINDEXCLASS_H
#define ISINDEXCLASS_H

#include "AttrClass.h"

// forward declarations

class IsIndexClass : public AttrClass
{
	public:
		IsIndexClass () : AttrClass() { Flags |= FLG_Gadget; }
		~IsIndexClass () { delete Prompt; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		VOID AppendGadget (struct AppendGadgetMessage &amsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID AdjustPosition (LONG x, LONG y);
		VOID Render (struct RenderMessage &rmsg);

	protected:
		LONG Left, Baseline;
		STRPTR Prompt;
		Object *MUIGadget;
		ULONG Width, Height;
};

#endif // ISINDEXCLASS_H
