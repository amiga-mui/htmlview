#ifndef BRCLASS_H
#define BRCLASS_H

#include "AttrClass.h"

// forward declarations

class BrClass : public AttrClass
{
	public:
		BrClass () : AttrClass() { Flags |= FLG_Newline; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		BOOL Mark (struct MarkMessage &mmsg);

	protected:
		WORD Clear;
};

#endif // BRCLASS_H
