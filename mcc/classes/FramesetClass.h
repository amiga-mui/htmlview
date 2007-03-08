#ifndef FRAMESETCLASS_H
#define FRAMESETCLASS_H

#include "TreeClass.h"

// forward declarations

class FramesetClass : public TreeClass
{
	public:
		FramesetClass () : TreeClass() { ; }
		~FramesetClass () { delete ColumnsStr; delete RowsStr; delete Columns; delete Rows; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		BOOL Layout (struct LayoutMessage &lmsg);
		Object *LookupFrame (STRPTR name, class HostClass *hclass);
		Object *HandleMUIEvent (struct MUIP_HandleEvent *hmsg);
		Object *FindDefaultFrame (ULONG &size);

  protected:
    ULONG *ConvertSizeList(STRPTR value_list, LONG total, ULONG &cnt);

	protected:
		LONG Left, Width;
		ULONG ColumnCnt, RowCnt;
		STRPTR ColumnsStr, RowsStr;
		ULONG *Columns, *Rows;
};

#endif // FRAMESETCLASS_H
