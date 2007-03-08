#ifndef OPTIONCLASS_H
#define OPTIONCLASS_H

#include "TreeClass.h"

// forward declarations

class OptionClass : public TreeClass
{
	public:
		OptionClass () : TreeClass() { ; }
		~OptionClass () { delete Value; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));

//	protected:
		STRPTR Value, Contents;
};

#endif // OPTIONCLASS_H
