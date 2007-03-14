
#include "MetaClass.h"
#include "HostClass.h"

#include "ParseMessage.h"
#include "ScanArgs.h"

VOID MetaClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	AttrClass::Parse(pmsg);

	STRPTR http = NULL, content = NULL;
	struct ArgList args[] =
	{
		{ "HTTP-EQUIV",	&http,		ARG_STRING, NULL },
		{ "CONTENT",		&content,	ARG_STRING, NULL },
		{ NULL,           NULL,       0,          NULL }
	};
	ScanArgs(pmsg.Locked, args);

	if(http && content && !stricmp(http, "REFRESH") && !pmsg.NoFrames)
	{
		FLOAT delay;
		ULONG offset = 0;
		if(sscanf(content, "%hf%*[,; ]%*[URLurl]=%n", &delay, &offset) == 4 && offset)
		{
			strcpy(pmsg.Host->RefreshURL = new char[strlen(content+offset)+1], content+offset);
			pmsg.Host->RefreshTime = (ULONG)(delay*1000);
		}
	}
	delete http;
	delete content;
}
