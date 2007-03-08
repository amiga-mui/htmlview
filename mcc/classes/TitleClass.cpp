
#include "TitleClass.h"
#include "TextClass.h"

#include "ParseMessage.h"

VOID TitleClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	TreeClass::Parse(pmsg);

	if(FirstChild && FirstChild->Obj && FirstChild->Obj->id() == tag_Text)
		pmsg.Title = ((class TextClass *)FirstChild->Obj)->Contents;
}
