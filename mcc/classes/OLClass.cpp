
#include "OLClass.h"

#include "Layout.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

BOOL OLClass::Layout (struct LayoutMessage &lmsg)
{
	UWORD old_type = lmsg.OL_Type;
	lmsg.OL_Type = Type;
	ULClass::Layout(lmsg);
	lmsg.OL_Type = old_type;
}

BOOL OLClass::Mark (struct MarkMessage &mmsg)
{
	UWORD old_type = mmsg.OL_Type;
	mmsg.OL_Type = Type;
	ULClass::Mark(mmsg);
	mmsg.OL_Type = old_type;
}

VOID OLClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	STRPTR type = NULL;
	LONG start = 1;
	struct ArgList args[] =
	{
		{ "TYPE",	&type,		ARG_STRING	},
		{ "START",	&start,		ARG_NUMBER	},
		{ NULL }
	};
	ScanArgs(pmsg.Locked, args);

	if(type)
	{
		switch(*type)
		{
			case 'a':
				Type = OL_LowerAlpha;
			break;

			case 'A':
				Type = OL_UpperAlpha;
			break;

			case 'i':
				Type = OL_LowerRoman;
			break;

			case 'I':
				Type = OL_UpperRoman;
			break;
		}
		delete type;
	}

	APTR handle;
	if(handle = Backup(pmsg, 1, tag_LI))
	{
		if(++pmsg.OL_Level < 3)
			pmsg.OL_Cnt[pmsg.OL_Level] = start;
		TreeClass::Parse(pmsg);
		pmsg.OL_Level--;
		Restore(pmsg.OpenCounts, 1, handle);
	}
}

VOID OLClass::Render (struct RenderMessage &rmsg)
{
	UWORD old_type = rmsg.OL_Type;
	rmsg.OL_Type = Type;
	ULClass::Render(rmsg);
	rmsg.OL_Type = old_type;
}

