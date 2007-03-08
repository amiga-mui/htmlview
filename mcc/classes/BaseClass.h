#ifndef BASECLASS_H
#define BASECLASS_H

#include "AttrClass.h"

// forward declarations

class BaseClass : public AttrClass
{
	public:
		BaseClass () : AttrClass() { ; }
		~BaseClass () { delete URL; delete Target; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));

		STRPTR URL, Target;
};

#endif // BASECLASS_H
