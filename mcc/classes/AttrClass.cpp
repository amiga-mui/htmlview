
#include "AttrClass.h"

#include "ParseMessage.h"

BOOL AttrClass::Layout (struct LayoutMessage &lmsg)
{
	SuperClass::Layout(lmsg);
	Flags |= FLG_Layouted;
}

VOID AttrClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.AdvancedNextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif
}

