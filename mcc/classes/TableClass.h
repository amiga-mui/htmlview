#ifndef TABLECLASS_H
#define TABLECLASS_H

#include "BackFillClass.h"
#include "MinMax.h"

// forward declarations

class TableClass : public BackFillClass
{
	public:
		TableClass()
      : BackFillClass()
    {
      Flags |= FLG_Newline;
    }
		
    ~TableClass()
    {
      delete Widths;
      delete Heights;
      delete GivenWidth;
      delete RowOpenCounts;
      FirstChild = RealFirstChild;
    }
		
    VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID Relayout (BOOL all);
		VOID AdjustPosition (LONG x, LONG y);
		VOID Render (struct RenderMessage &rmsg);
		class SuperClass *FindElement (ULONG tagID);
		BOOL Mark (struct MarkMessage &mmsg);
		VOID Spread (LONG scale, LONG width, LONG (*get_width) (struct CellWidth *));

  protected:
    VOID DrawCorner(struct RastPort *rport, LONG x, LONG y, UWORD width, UBYTE col1, UBYTE col2);

	protected:
		ULONG Rows, Columns, Min, Max, Width, Height;
		ULONG *Heights, *RowOpenCounts;
		struct CellWidth *Widths;
		LONG Left, Spacing, Padding, BorderSize, Alignment;
		struct ArgSize *GivenWidth;
		struct ChildsList *RealFirstChild;
};

#endif // TABLECLASS_H
