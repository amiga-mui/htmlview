#ifndef TEXTAREACLASS_H
#define TEXTAREACLASS_H

#include "TreeClass.h"

// forward declarations

class TextAreaClass : public TreeClass
{
	public:
		TextAreaClass () : TreeClass() { Flags |= FLG_Gadget; }
		~TextAreaClass () { delete Name; delete Contents; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID AppendGadget (struct AppendGadgetMessage &amsg);
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID AdjustPosition (LONG x, LONG y);
		VOID Render (struct RenderMessage &rmsg);
		VOID ExportForm (struct ExportFormMessage &emsg);
		VOID ResetForm ();

	protected:
		STRPTR Name, Contents;
		LONG Left, Baseline;
		ULONG Columns, Rows, Width, Height;
		Object *MUIGadget;
};

#endif // TEXTAREACLASS_H
