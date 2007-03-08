
#include "PClass.h"

#include "Layout.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

BOOL PClass::Layout (struct LayoutMessage &lmsg)
{
	if(Flags & FLG_Virgin)
	{
		lmsg.EnsureNewline();
		if(lmsg.MinY != lmsg.Y)
		{
			lmsg.UpdateBaseline(lmsg.Font->tf_YSize+1, lmsg.Font->tf_Baseline);
			lmsg.Newline();
		}
	}

	UBYTE oldalign = lmsg.Align;
	if(Alignment)
		lmsg.Align = Alignment;

	if(TreeClass::Layout(lmsg))
		lmsg.EnsureNewline();

	lmsg.Align = oldalign;
}

BOOL PClass::Mark (struct MarkMessage &mmsg)
{
	if(mmsg.Enabled())
	{
		if(!mmsg.Newline)
			mmsg.WriteLF();
		mmsg.WriteLF();
		mmsg.Newline = TRUE;
	}
	return(TreeClass::Mark(mmsg));
}

VOID PClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	struct ArgList args[] =
	{
		{ "ALIGN",	&Alignment,		ARG_KEYWORD, AlignKeywords },
		{ NULL }
	};

	Alignment = -1;
	ScanArgs(pmsg.Locked, args);
	Alignment++;

	TreeClass::Parse(pmsg);
}

