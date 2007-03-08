#ifndef DTCLASS_H
#define DTCLASS_H

#include "DummyClass.h"

// forward declarations

class DTClass : public DummyClass
{
	public:
		DTClass () : DummyClass() { Flags |= FLG_Newline; Group = group_Definition; }
		BOOL Layout (struct LayoutMessage &lmsg);
};

#endif // DTCLASS_H
