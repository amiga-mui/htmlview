
#include "BaseClass.h"
#include "HostClass.h"

#include "ParseMessage.h"
#include "ScanArgs.h"

VOID BaseClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	AttrClass::Parse(pmsg);
	pmsg.SkipSpaces();

	struct ArgList args[] =
	{
		{ "HREF",	  &URL,		  ARG_URL, NULL	},
		{ "TARGET",	&Target,	ARG_URL, NULL	},
		{ NULL,     NULL,     0,       NULL }
	};
	ScanArgs(pmsg.Locked, args);

	if(URL)
	{
		ULONG last, i = last = 0;
		while(URL[i])
		{
			if(URL[i++] == '/')
				last = i;
		}
		URL[last] = '\0';
	}

	pmsg.Host->Base = this;
}
