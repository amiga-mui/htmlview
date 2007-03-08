
#include "ULClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "SharedData.h"

BOOL ULClass::Layout (struct LayoutMessage &lmsg)
{
	if(Flags & FLG_Virgin)
	{
		lmsg.AddYSpace(4);
		lmsg.Indent += 20;
		lmsg.X += 20;
	}

	ULONG old_indent = lmsg.LIIndent;
	lmsg.LIIndent = lmsg.Share->LI_Width;

	if(TreeClass::Layout(lmsg))
	{
		lmsg.AddYSpace(4);
		lmsg.Indent -= 20;
		lmsg.X -= 20;
	}

	lmsg.LIIndent = old_indent;
}

VOID ULClass::MinMax (struct MinMaxMessage &mmsg)
{
	mmsg.Indent += 20;
	TreeClass::MinMax(mmsg);
	mmsg.X -= 20;
	mmsg.Indent -= 20;
}

VOID ULClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	APTR handle;
	if(handle = Backup(pmsg, 2, tag_LI, tag_OL))
	{
		TreeClass::Parse(pmsg);
		Restore(pmsg.OpenCounts, 2, handle);
	}
}

VOID ULClass::Render (struct RenderMessage &rmsg)
{
	rmsg.UL_Nesting++;
	TreeClass::Render(rmsg);
	rmsg.UL_Nesting--;
}

