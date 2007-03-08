#ifndef HRCLASS_H
#define HRCLASS_H

#include "AttrClass.h"

// forward declarations

class HRClass : public AttrClass
{
	public:
		HRClass () : AttrClass() { Flags |= FLG_Newline; }
		~HRClass () { delete GivenWidth; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID AdjustPosition (LONG x, LONG y);
		VOID Render (struct RenderMessage &rmsg);
		BOOL Mark (struct MarkMessage &mmsg);

	protected:
		LONG Left, Right;
		ULONG Alignment, Height;
		struct ArgSize *GivenWidth;
};

#endif // HRCLASS_H
