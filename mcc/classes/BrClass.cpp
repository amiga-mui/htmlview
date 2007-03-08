
#include "BrClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

BOOL BrClass::Layout (struct LayoutMessage &lmsg)
{
	if(lmsg.Baseline + lmsg.Bottom == 0)
		lmsg.UpdateBaseline(lmsg.Font->tf_YSize+1, lmsg.Font->tf_Baseline);
	lmsg.Newline();
	lmsg.FlushImages(Clear);

	AttrClass::Layout(lmsg);
}

BOOL BrClass::Mark (struct MarkMessage &mmsg)
{
	BOOL result = SuperClass::Mark(mmsg);

	mmsg.Newline = TRUE;
	if(mmsg.Enabled())
		mmsg.WriteLF();

	return(result);
}

VOID BrClass::MinMax (struct MinMaxMessage &mmsg)
{
	mmsg.Newline();

	Flags |= FLG_KnowsMinMax;
}

VOID BrClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	AttrClass::Parse(pmsg);
	pmsg.SkipSpaces();

	STRPTR ClearKeywords[] = { "LEFT", "RIGHT", "ALL", NULL };
	ULONG clear = -1;
	struct ArgList args[] =
	{
		{ "CLEAR",	&clear,	ARG_KEYWORD, ClearKeywords	},
		{ NULL }
	};
	ScanArgs(pmsg.Locked, args);
	Clear = clear+1;
}

