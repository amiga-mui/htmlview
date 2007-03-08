
#include "ScriptClass.h"

#include "ParseMessage.h"

VOID ScriptClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
#ifdef OUTPUT
	PrintTag("<SCRIPT>");
#endif

	do {

		pmsg.NextEndBracket();
		if(*pmsg.Current != '<')
			pmsg.NextStartBracket();

	} while (pmsg.Fetch(10) && strnicmp(pmsg.Current+1, "/SCRIPT>", 7));
}
