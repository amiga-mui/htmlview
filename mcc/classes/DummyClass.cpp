
#include "DummyClass.h"

#include "ParseMessage.h"

VOID DummyClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	TreeClass::Parse(pmsg);
}
