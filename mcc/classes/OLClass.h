#ifndef OLCLASS_H
#define OLCLASS_H

#include "ULClass.h"

// forward declarations

class OLClass : public ULClass
{
	public:
		OLClass () : ULClass() { ; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID Render (struct RenderMessage &rmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		BOOL Mark (struct MarkMessage &mmsg);

	protected:
		UWORD Type;
};

#endif // OLCLASS_H
