
#include "NoFramesClass.h"

#include "ParseMessage.h"

VOID NoFramesClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	pmsg.NoFrames = TRUE;
	TreeClass::Parse(pmsg);
	pmsg.NoFrames = FALSE;
}
