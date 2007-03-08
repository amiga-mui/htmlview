#ifndef TITLECLASS_H
#define TITLECLASS_H

#include "TreeClass.h"

// forward declarations

class TitleClass : public TreeClass
{
	public:
		TitleClass () : TreeClass() { ; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
};

#endif // TITLECLASS_H
