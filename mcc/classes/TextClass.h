#ifndef TEXTCLASS_H
#define TEXTCLASS_H

#include "SuperClass.h"

// forward declarations

class TextClass : public SuperClass
{
	public:
		TextClass () : SuperClass() { ID = tag_Text; LineInfoLast = (struct TextLineInfo *)&LineInfo; }
		~TextClass ()
		{
			delete Contents;
			DeleteLineInfo();
		}

		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID Relayout (BOOL all);
		VOID AdjustPosition (LONG x, LONG y);
		VOID Render (struct RenderMessage &rmsg);
		BOOL HitTest (struct HitTestMessage &hmsg);
		VOID DeleteLineInfo ();
		class TextClass *Find (struct FindMessage &fmsg);
		BOOL Mark (struct MarkMessage &mmsg);
		LONG FindChar (LONG x, LONG y, BOOL newline);
		VOID RenderMarked (struct RastPort *rp, LONG markbegin, LONG markend, LONG xoffset, LONG yoffset);

//	protected:
		struct TextLineInfo *LineInfo;
		struct TextLineInfo *LineInfoLast;
		LONG Left;
		STRPTR Contents;
		ULONG Length;
		ULONG MarkBegin, MarkEnd;
		struct TextFont *Font;
};

#endif // TEXTCLASS_H
