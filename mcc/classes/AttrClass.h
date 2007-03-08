#ifndef ATTRCLASS_H
#define ATTRCLASS_H

#include "SuperClass.h"

// forward declarations

class AttrClass : public SuperClass
{
	public:
		AttrClass () : SuperClass() { Flags |= FLG_Attribute; }
		virtual ~AttrClass () { ; }

		virtual VOID Parse (REG(a2, struct ParseMessage &pmsg));
		virtual BOOL Layout (struct LayoutMessage &lmsg);
};

#endif // ATTRCLASS_H
