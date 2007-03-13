#ifndef SUPERCLASS_H
#define SUPERCLASS_H

#include "Classes.h"

#include <exec/types.h>
#include <clib/alib_protos.h>
#include <proto/intuition.h>

// forward declarations
class HostClass;

class SuperClass
{
  public:
		SuperClass() {}
    virtual ~SuperClass()	{}
		
    virtual VOID Parse(REG(a2, struct ParseMessage& pmsg)) = 0;
		virtual BOOL Layout(struct LayoutMessage& lmsg) = 0;

		virtual VOID AppendGadget(struct AppendGadgetMessage& amsg UNUSED) {}
		virtual VOID MinMax(struct MinMaxMessage& mmsg UNUSED) {}
		virtual VOID Render(struct RenderMessage& rmsg UNUSED) {}
		virtual VOID AllocateColours(struct ColorMap* cmap UNUSED) {}
		virtual VOID FreeColours(struct ColorMap* cmap UNUSED) {}
		virtual VOID ExportForm(struct ExportFormMessage& emsg UNUSED) {}
		virtual VOID ResetForm() {}
		virtual BOOL HitTest(struct HitTestMessage& hmsg UNUSED) { return FALSE; }
		virtual Object* LookupFrame(STRPTR name UNUSED, class HostClass* hclass UNUSED)	 { return NULL; }
		virtual Object* HandleMUIEvent(struct MUIP_HandleEvent* hmsg UNUSED) { return NULL; }
		virtual Object* FindDefaultFrame(ULONG& size UNUSED) { return NULL; }
		virtual class MapClass *FindMap(STRPTR name UNUSED) { return NULL; }
		virtual BOOL UseMap(struct UseMapMessage& umsg UNUSED) { return FALSE; }
		virtual class TextClass* Find(struct FindMessage& fmsg UNUSED) { return NULL; }
		virtual VOID GetImages(struct GetImagesMessage& gmsg UNUSED)  {}
		virtual BOOL ReceiveImage(struct PictureFrame* pic UNUSED) { return FALSE ; }
		virtual BOOL UpdateImage(LONG ystart UNUSED, LONG ystop UNUSED, LONG top UNUSED, LONG bottom UNUSED, BOOL last UNUSED) { return(FALSE); }
		virtual BOOL FlushImage(LONG top UNUSED, LONG bottom UNUSED) { return(FALSE); }
		virtual VOID DrawBackground(struct RenderMessage& rmsg UNUSED, LONG left UNUSED,
                                LONG top UNUSED, LONG width UNUSED, LONG height UNUSED,
                                LONG xoffset UNUSED, LONG yoffset UNUSED) {}

		virtual VOID AdjustPosition(LONG x, LONG y);
		virtual VOID Relayout(BOOL all);
		virtual WORD InDomain(struct RenderMessage& rmsg);
		virtual class AClass *FindAnchor(STRPTR name);
		virtual class SuperClass *FindElement(ULONG tagID);
		virtual BOOL Mark(struct MarkMessage &mmsg);

    UWORD flags(void) const   { return Flags; }
    UBYTE id(void) const      { return ID;    }
    UBYTE group(void) const   { return Group; }
    UBYTE top(void) const     { return Top;   }
    UBYTE bottom(void) const  { return Bottom;}

    void setFlags(const UWORD flags) { Flags = flags;   }
    void setId(const UBYTE id)       { ID = id;         }
    void setGroup(const UBYTE group) { Group = group;   }
    void setTop(const LONG top)      { Top = top;       }
    void setBottom(const LONG bottom){ Bottom = bottom; }

  protected:
		UWORD Flags;
		UBYTE ID;
    UBYTE Group;
		LONG Top;
    LONG Bottom;
};

#endif // SUPERCLASS_H
