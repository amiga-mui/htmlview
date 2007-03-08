#ifndef LICLASS_H
#define LICLASS_H

#include "TreeClass.h"

// forward declarations

class LIClass : public TreeClass
{
	public:
		LIClass () : TreeClass() { Flags |= FLG_Newline; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID AdjustPosition (LONG x, LONG y);
		VOID Render (struct RenderMessage &rmsg);
		BOOL Mark (struct MarkMessage &mmsg);
		VOID NumToStr (STRPTR str, UWORD type);

  protected:
    VOID DecToRoman(ULONG number, STRPTR romanp);
    STRPTR DigitToRoman(STRPTR romanp, UBYTE number, UBYTE a, UBYTE b, UBYTE c);

	protected:
		LONG Left, Baseline;
		ULONG Number;
};

#endif // LICLASS_H
