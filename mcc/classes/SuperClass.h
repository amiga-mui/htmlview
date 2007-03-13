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

		virtual VOID AppendGadget(struct AppendGadgetMessage&) {}
		virtual VOID MinMax(struct MinMaxMessage&) {}
		virtual VOID Render(struct RenderMessage&) {}
		virtual VOID AllocateColours(struct ColorMap*) {}
		virtual VOID FreeColours(struct ColorMap*) {}
		virtual VOID ExportForm(struct ExportFormMessage&) {}
		virtual VOID ResetForm() {}
		virtual BOOL HitTest(struct HitTestMessage&) { return FALSE; }
		virtual Object* LookupFrame(STRPTR, class HostClass*)	 { return NULL; }
		virtual Object* HandleMUIEvent(struct MUIP_HandleEvent*) { return NULL; }
		virtual Object* FindDefaultFrame(ULONG&) { return NULL; }
		virtual class MapClass *FindMap(STRPTR) { return NULL; }
		virtual BOOL UseMap(struct UseMapMessage&) { return FALSE; }
		virtual class TextClass* Find(struct FindMessage&) { return NULL; }
		virtual VOID GetImages(struct GetImagesMessage&)  {}
		virtual BOOL ReceiveImage(struct PictureFrame*) { return FALSE ; }
		virtual BOOL UpdateImage(LONG, LONG, LONG, LONG, BOOL) { return(FALSE); }
		virtual BOOL FlushImage(LONG, LONG) { return(FALSE); }
		virtual VOID DrawBackground(struct RenderMessage&, LONG,
                                LONG, LONG, LONG,
                                LONG, LONG) {}

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
