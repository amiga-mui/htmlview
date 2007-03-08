#ifndef THCLASS_H
#define THCLASS_H

#include "TDClass.h"

// forward declarations

class THClass : public TDClass
{
	public:
		THClass () : TDClass() { ; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID TDRender (struct RenderMessage &rmsg);
};

#endif // THCLASS_H
