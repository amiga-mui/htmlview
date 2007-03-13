
#include "DTClass.h"

#include "Layout.h"

BOOL DTClass::Layout (struct LayoutMessage &lmsg)
{
	if(Flags & FLG_Virgin)
		lmsg.EnsureNewline();

	if(TreeClass::Layout(lmsg))
		lmsg.EnsureNewline();

  return TRUE;
}
