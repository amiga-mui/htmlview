#ifndef DUMMYCLASS_H
#define DUMMYCLASS_H

#include "SuperClass.h"
#include "TreeClass.h"

// forward declarations

class DummyClass : public TreeClass
{
	public:
		DummyClass () : TreeClass() { ; }

		virtual VOID Parse (REG(a2, struct ParseMessage &pmsg));
};

#endif // DUMMYCLASS_H
