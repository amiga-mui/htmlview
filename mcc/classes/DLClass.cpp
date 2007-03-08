
#include "DLClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"

BOOL DLClass::Layout (struct LayoutMessage &lmsg)
{
	if(Flags & FLG_Virgin)
	{
		lmsg.AddYSpace(4);
		lmsg.X += 5;
		lmsg.Indent += 5;
	}

	if(TreeClass::Layout(lmsg))
	{
		lmsg.AddYSpace(4);
		lmsg.X -= 5;
		lmsg.Indent -= 5;
	}
}

VOID DLClass::MinMax (struct MinMaxMessage &mmsg)
{
	mmsg.Indent += 5;
	TreeClass::MinMax(mmsg);
	mmsg.X -= 5;
	mmsg.Indent -= 5;
}

VOID DLClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	UBYTE group = pmsg.OpenGroups[group_Definition];
	pmsg.OpenGroups[group_Definition] = 0;
	TreeClass::Parse(pmsg);
	pmsg.OpenGroups[group_Definition] = group;
}

