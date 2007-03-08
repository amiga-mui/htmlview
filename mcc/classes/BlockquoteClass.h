#ifndef BLOCKQUOTECLASS_H
#define BLOCKQUOTECLASS_H

#include "DummyClass.h"

// forward declarations

class BlockquoteClass : public DummyClass
{
	public:
		BlockquoteClass () : DummyClass() { Flags |= FLG_Newline; }
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
};

#endif // BLOCKQUOTECLASS_H
