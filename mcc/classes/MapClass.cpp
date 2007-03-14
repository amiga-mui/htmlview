
#include "MapClass.h"

#include "Map.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

class MapClass *MapClass::FindMap (STRPTR name)
{
	return(stricmp(Name, name+1) ? TreeClass::FindMap(name) : this);
}

BOOL MapClass::UseMap (struct UseMapMessage &umsg)
{
	return(TreeClass::UseMap(umsg) && umsg.URL);
}

VOID MapClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	struct ArgList args[] =
	{
		{ "NAME",	&Name,	ARG_URL, NULL },
		{ NULL,     NULL,    0,       NULL }
	};
	ScanArgs(pmsg.Locked, args);

	TreeClass::Parse(pmsg);
}

