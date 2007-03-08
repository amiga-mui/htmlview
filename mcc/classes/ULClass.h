#ifndef ULCLASS_H
#define ULCLASS_H

#include "TreeClass.h"

// forward declarations

class ULClass : public TreeClass
{
	public:
		ULClass () : TreeClass() { Flags |= FLG_Newline; }
		virtual VOID Parse (REG(a2, struct ParseMessage &pmsg));
		virtual VOID MinMax (struct MinMaxMessage &mmsg);
		virtual BOOL Layout (struct LayoutMessage &lmsg);
		virtual VOID Render (struct RenderMessage &rmsg);
};

#endif // ULCLASS_H
