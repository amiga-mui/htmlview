#ifndef METACLASS_H
#define METACLASS_H

#include "AttrClass.h"

// forward declarations

class MetaClass : public AttrClass
{
	public:
		MetaClass () : AttrClass() { ; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
};

#endif // METACLASS_H
