#ifndef FORMCLASS_H
#define FORMCLASS_H

#include "TreeClass.h"

// forward declarations

class FormClass : public TreeClass
{
	public:
		FormClass () : TreeClass() { ; }
		~FormClass () { delete Action; delete EncType; delete Target; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID AppendGadget (struct AppendGadgetMessage &amsg);
		VOID ExportForm (struct ExportFormMessage &emsg);
		VOID ResetForm ();

	protected:
		STRPTR Action, EncType, Target;
		ULONG Method;
		Object *HTMLview;
};

#endif // FORMCLASS_H
