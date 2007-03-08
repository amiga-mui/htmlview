#ifndef SCRIPTCLASS_H
#define SCRIPTCLASS_H

#include "AttrClass.h"

// forward declarations

class ScriptClass : public AttrClass
{
	public:
		ScriptClass () : AttrClass() { ; }
		~ScriptClass () { delete Script; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));

	private:
		STRPTR Script;
};

#endif // SCRIPTCLASS_H
