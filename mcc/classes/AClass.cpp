
#include "AClass.h"

#include "ParseMessage.h"
#include "ScanArgs.h"

BOOL AClass::HitTest (struct HitTestMessage &hmsg)
{
	if(URL)
	{
		class SuperClass *oldlink = hmsg.Obj;
		hmsg.Obj = this;
		hmsg.URL = URL;
		hmsg.Target = Target;

		if(TreeClass::HitTest(hmsg))
			return(TRUE);

		hmsg.Obj = oldlink;
		return(FALSE);
	}
	else
	{
		return(TreeClass::HitTest(hmsg));
	}
}

class AClass *AClass::FindAnchor (STRPTR name)
{
	return((Name && !strcmp(Name, name)) ? this : TreeClass::FindAnchor(name));
}

VOID AClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	struct ArgList args[] =
	{
		{ "HREF",	  &URL,		  ARG_URL, NULL	},
		{ "NAME",	  &Name,	  ARG_URL, NULL	},
		{ "TARGET",	&Target,  ARG_URL, NULL	},
		{ NULL,     NULL,     0,       NULL }
	};
	ScanArgs(pmsg.Locked, args);

	BOOL old_link = pmsg.Linkable;
	pmsg.Linkable = URL ? TRUE : FALSE;
	TreeClass::Parse(pmsg);
	pmsg.Linkable = old_link;
}

VOID AClass::Render (struct RenderMessage &rmsg)
{
	if(!(Flags & FLG_A_QueryDone))
	{
		STRPTR url;
		if(URL && (url = (STRPTR)DoMethod(rmsg.HTMLview, MUIM_HTMLview_AddPart, URL)))
		{
			if(DoMethod(rmsg.HTMLview, MUIM_HTMLview_VLink, url))
				Flags |= FLG_A_Visited;
			delete url;
		}
		Flags |= FLG_A_QueryDone;
	}

	UBYTE oldstyles = rmsg.Textstyles;
	rmsg.Textstyles |= (URL ? (Flags & FLG_A_Visited ? TSF_Link | TSF_VLink : TSF_Link) : 0);

	LONG oldcol = rmsg.Colours[Col_Text];
	if(rmsg.Textstyles & TSF_Link)
		rmsg.Colours[Col_Text] = rmsg.Colours[(rmsg.Textstyles & TSF_VLink) ? Col_VLink : Col_Link];

	if(rmsg.TargetObj == (class SuperClass *)this)
	{
		rmsg.TargetObj = NULL;
		TreeClass::Render(rmsg);
		rmsg.TargetObj = this;
	}
	else
	{
		TreeClass::Render(rmsg);
	}

	rmsg.Colours[Col_Text] = oldcol;
	rmsg.Textstyles = oldstyles;
}

