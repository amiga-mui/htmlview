#ifndef MAPCLASS_H
#define MAPCLASS_H

#include "TreeClass.h"

// forward declarations

class MapClass : public TreeClass
{
	public:
		MapClass () : TreeClass() { ; }
		~MapClass () { delete Name; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		class MapClass *FindMap (STRPTR name);
		BOOL UseMap (struct UseMapMessage &umsg);

	protected:
		STRPTR Name;
};

#endif // MAPCLASS_H
