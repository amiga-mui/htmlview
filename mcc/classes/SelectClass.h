#ifndef SELECTCLASS_H
#define SELECTCLASS_H

#include "TreeClass.h"

// forward declarations

class SelectClass : public TreeClass
{
	public:
		SelectClass () : TreeClass() { Flags |= FLG_Gadget; }
		~SelectClass () { delete Name; delete Entries; delete Values; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID AppendGadget (struct AppendGadgetMessage &amsg);
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID AdjustPosition (LONG x, LONG y);
		VOID Render (struct RenderMessage &rmsg);
		VOID ExportForm (struct ExportFormMessage &emsg);
		VOID ResetForm ();

	protected:
		STRPTR Name, *Entries, *Values;
		LONG Left, Baseline;
		ULONG Active, Size, Width, Height;
		Object *MUIGadget;
};

#endif // SELECTCLASS_H
