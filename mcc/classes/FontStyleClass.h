#ifndef FONTSTYLECLASS_H
#define FONTSTYLECLASS_H

#include "TreeClass.h"

// forward declarations

class FontStyleClass : public TreeClass
{
	public:
		FontStyleClass (UBYTE textstyles, UBYTE alignment, BOOL newline, BYTE font);
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID Render (struct RenderMessage &rmsg);
		WORD InDomain (struct RenderMessage &rmsg);

	protected:
		UBYTE Textstyles, Alignment;
		BYTE Font;
};

#endif // FONTSTYLECLASS_H
