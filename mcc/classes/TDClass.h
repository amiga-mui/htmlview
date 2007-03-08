#ifndef TDCLASS_H
#define TDCLASS_H

#include "BackFillClass.h"

// forward declarations

class TDClass : public BackFillClass
{
	public:
		TDClass () : BackFillClass()
		{
			Group = group_Tablecell;
			Flags |= FLG_Newline;
			ColSpan = RowSpan = 1;
			VAlignment = Align_Middle;
		}
		~TDClass () { delete GivenWidth; }
		virtual VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID TDMinMax (struct MinMaxMessage &mmsg);
		BOOL TDLayout (struct LayoutMessage &lmsg);
		VOID AdjustPosition (LONG x, LONG y);
		virtual VOID TDRender (struct RenderMessage &rmsg);
		BOOL Mark (struct MarkMessage &mmsg);

		ULONG Width, Height, Min, Max;
		ULONG ColSpan, RowSpan;
		LONG Left, Alignment, VAlignment;
		struct ArgSize *GivenWidth;
};

#endif // TDCLASS_H
