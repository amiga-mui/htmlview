#ifndef FRAMECLASS_H
#define FRAMECLASS_H

#include "AttrClass.h"

// forward declarations

class FrameClass : public AttrClass
{
	public:
		FrameClass () : AttrClass() { ; }
		~FrameClass () { delete Name; delete Src; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID AppendGadget (struct AppendGadgetMessage &amsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID Render (struct RenderMessage &rmsg);
		Object *LookupFrame (STRPTR name, class HostClass *hclass);
		Object *HandleMUIEvent (struct MUIP_HandleEvent *hmsg);
		BOOL HitTest (struct HitTestMessage &hmsg);
		Object *FindDefaultFrame (ULONG &size);

	protected:
		STRPTR Name, Src;
		LONG Left, Right;
		ULONG Scrolling, FrameBorder;
		ULONG MarginWidth, MarginHeight;
		Object *ScrollGroup, *HTMLview;
};

#endif // FRAMECLASS_H
