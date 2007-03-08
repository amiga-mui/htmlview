
#include "THClass.h"

#include "ParseMessage.h"

VOID THClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	TDClass::Parse(pmsg);
	if(Alignment == Align_None)
		Alignment = Align_Center;
}

VOID THClass::TDRender (struct RenderMessage &rmsg)
{
	UBYTE oldstyles = rmsg.Textstyles;
	rmsg.Textstyles |= TSF_Bold;
	TDClass::TDRender(rmsg);
	rmsg.Textstyles = oldstyles;
}

