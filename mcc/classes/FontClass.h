#ifndef FONTCLASS_H
#define FONTCLASS_H

#include "TreeClass.h"

// forward declarations

class FontClass : public TreeClass
{
	public:
		FontClass () : TreeClass() { FontRGB = -1; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID Render (struct RenderMessage &rmsg);
		WORD InDomain (struct RenderMessage &rmsg) { return(InsideLayer); }
		VOID AllocateColours (struct ColorMap *cmap);
		VOID FreeColours (struct ColorMap *cmap);

	protected:
		LONG FontRGB, FontCol;
		BYTE Font;
};

#endif // FONTCLASS_H
