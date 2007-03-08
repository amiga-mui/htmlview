#ifndef NOFRAMESCLASS_H
#define NOFRAMESCLASS_H

#include "TreeClass.h"

// forward declarations

class NoFramesClass : public TreeClass
{
	public:
		NoFramesClass () : TreeClass() { ; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
};

#endif // NOFRAMESCLASS_H
