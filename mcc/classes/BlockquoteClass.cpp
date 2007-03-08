
#include "BlockquoteClass.h"

#include "Layout.h"
#include "MinMax.h"

BOOL BlockquoteClass::Layout (struct LayoutMessage &lmsg)
{
	if(Flags & FLG_Virgin)
	{
		lmsg.EnsureNewline();
		if(lmsg.MinY != lmsg.Y)
			lmsg.AddYSpace(4);
		lmsg.Indent += 30;
		lmsg.X += 30;
		lmsg.MaxX -= 30;
	}

	if(TreeClass::Layout(lmsg))
	{
		lmsg.AddYSpace(4);
		lmsg.Indent -= 30;
		lmsg.X -= 30;
		lmsg.MaxX += 30;
	}
}

VOID BlockquoteClass::MinMax (struct MinMaxMessage &mmsg)
{
	mmsg.Indent += 60;
	TreeClass::MinMax(mmsg);
	mmsg.X -= 60;
	mmsg.Indent -= 60;
}

