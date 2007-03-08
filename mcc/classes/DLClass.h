#ifndef DLCLASS_H
#define DLCLASS_H

#include "TreeClass.h"

// forward declarations

class DLClass : public TreeClass
{
	public:
		DLClass () : TreeClass() { Flags |= FLG_Newline; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
};

#endif // DLCLASS_H
