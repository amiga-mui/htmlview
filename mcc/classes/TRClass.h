#ifndef TRCLASS_H
#define TRCLASS_H

#include "BackFillClass.h"

// forward declarations

class TRClass : public BackFillClass
{
	public:
		TRClass () : BackFillClass() { Flags |= FLG_Newline; }
		~TRClass () { FirstChild = RealFirstChild; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID TRMinMax (struct MinMaxMessage &mmsg);
		BOOL TRLayout (struct LayoutMessage &lmsg);
		VOID CountCells (struct CountCellsMessage &cmsg);
		VOID TRRender (struct RenderMessage &rmsg);
		VOID Verify ();
		BOOL Mark (struct MarkMessage &mmsg);

		struct ChildsList *RealFirstChild;
};

#endif // TRCLASS_H
