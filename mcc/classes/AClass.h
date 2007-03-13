#ifndef ACLASS_H
#define ACLASS_H

#include "TreeClass.h"

// forward declarations

class AClass : public TreeClass
{
	public:
		AClass () : TreeClass() { ; }
		~AClass () { delete URL; delete Target; delete Name; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID Render (struct RenderMessage &rmsg);
		WORD InDomain (struct RenderMessage&) { return(InsideLayer); }
		BOOL HitTest (struct HitTestMessage &hmsg);
		class AClass *FindAnchor (STRPTR name);

	protected:
		STRPTR URL, Name, Target;
};

#endif // ACLASS_H
