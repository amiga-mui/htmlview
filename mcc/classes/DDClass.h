#ifndef DDCLASS_H
#define DDCLASS_H

#include "DummyClass.h"

// forward declarations

class DDClass : public DummyClass
{
	public:
		DDClass () : DummyClass() { Flags |= FLG_Newline; Group = group_Definition; }
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
};

#endif // DDCLASS_H
