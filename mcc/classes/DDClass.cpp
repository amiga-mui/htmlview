
#include "DDClass.h"

#include "Layout.h"
#include "MinMax.h"

BOOL DDClass::Layout (struct LayoutMessage &lmsg)
{
	if(Flags & FLG_Virgin)
	{
		lmsg.EnsureNewline();
		lmsg.X += 15;
		lmsg.Indent += 15;

		lmsg.MinY = lmsg.Y;	// This indicates that no additional linebreaks should be placed infront of P-tags etc.
	}

	if(TreeClass::Layout(lmsg))
	{
		lmsg.EnsureNewline();
		lmsg.X -= 15;
		lmsg.Indent -= 15;
	}
}

VOID DDClass::MinMax (struct MinMaxMessage &mmsg)
{
	mmsg.Indent += 15;
	TreeClass::MinMax(mmsg);
	mmsg.X -= 15;
	mmsg.Indent -= 15;
}

