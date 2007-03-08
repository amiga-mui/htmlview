#ifndef INPUTCLASS_H
#define INPUTCLASS_H

#include "AttrClass.h"

// forward declarations

class InputClass : public AttrClass
{
	public:
		InputClass () : AttrClass() { Flags |= FLG_Gadget; }
		~InputClass () { delete Name; delete Value; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		VOID AppendGadget (struct AppendGadgetMessage &amsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID AdjustPosition (LONG x, LONG y);
		VOID Render (struct RenderMessage &rmsg);
		VOID ExportForm (struct ExportFormMessage &emsg);
		VOID ResetForm ();

	protected:
		LONG Left, Baseline;
		STRPTR Name, Value;
		Object *MUIGadget;
		ULONG Type, Size, MaxLength, Width, Height;
};

#endif // INPUTCLASS_H
