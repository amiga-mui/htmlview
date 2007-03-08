#ifndef PCLASS_H
#define PCLASS_H

#include "TreeClass.h"

// forward declarations

class PClass : public TreeClass
{
	public:
		PClass () : TreeClass() { Flags |= FLG_Newline; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		BOOL Layout (struct LayoutMessage &lmsg);
		BOOL Mark (struct MarkMessage &mmsg);

	protected:
		ULONG Alignment;
};

#endif // PCLASS_H
