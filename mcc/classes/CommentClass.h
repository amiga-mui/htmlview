#ifndef COMMENTCLASS_H
#define COMMENTCLASS_H

#include "AttrClass.h"

// forward declarations

class CommentClass : public AttrClass
{
	public:
		CommentClass () : AttrClass() { ; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
};

#endif // COMMENTCLASS_H
