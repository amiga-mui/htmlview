#ifndef HOSTCLASS_H
#define HOSTCLASS_H

#include "TreeClass.h"

// forward declarations

class HostClass : public TreeClass
{
	public:
		HostClass (Object *obj, struct HTMLviewData *data) : TreeClass()
		{
			FindMsg.MarkMsg = &MarkMsg;
			Obj = obj;
			Data = data;
		}
		~HostClass ();
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID Relayout (BOOL all)							{ if(Body) Body->Relayout(all); }
		VOID GetImages (struct GetImagesMessage &gmsg) { if(Body) Body->GetImages(gmsg); }
		VOID Render (struct RenderMessage &rmsg)		{ if(Body) Body->Render(rmsg); }
		VOID AllocateColours (struct ColorMap *cmap)	{ if(Body) Body->AllocateColours(cmap); }
		VOID FreeColours (struct ColorMap *cmap)		{ if(Body) Body->FreeColours(cmap); }
		class AClass *FindAnchor (STRPTR name)			{ if(Body) return(Body->FindAnchor(name)); else return(NULL); }
		Object *LookupFrame (STRPTR name, class HostClass *hclass);
		ULONG HandleEvent (Object *obj, struct IClass *cl, struct MUIP_HandleEvent *emsg);
		BOOL HitTest (struct HitTestMessage &hmsg);
		struct FindMessage *Find (STRPTR str, ULONG top, ULONG flags);
		VOID Mark (LONG x, LONG y);
		VOID UnMark ();
		VOID Copy ();

		class TreeClass *Body;
		class BaseClass *Base;
		Object *DefaultFrame;

		Object *Obj;
		struct HTMLviewData *Data;

		struct FindMessage FindMsg;
		struct MarkMessage MarkMsg;

		ULONG RefreshTime;
		STRPTR RefreshURL;

  protected:
    ULONG NumPadMovement(struct IntuiMessage *imsg);
    BOOL MatchKey(struct IntuiMessage *imsg, struct InputEvent &event, struct InputXpression &key);
    VOID RedrawLink(Object *obj, class SuperClass *redrawobj, struct HTMLviewData *data);
    class TreeClass *FindBody(struct ChildsList *first);
};

#endif // HOSTCLASS_H
