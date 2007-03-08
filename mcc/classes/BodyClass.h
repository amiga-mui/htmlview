#ifndef BODYCLASS_H
#define BODYCLASS_H

#include "BackFillClass.h"

// forward declarations

class BodyClass : public BackFillClass
{
	public:
		BodyClass () : BackFillClass() { TextRGB = LinkRGB = VLinkRGB = ALinkRGB = -1; }
		~BodyClass ();
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID Render (struct RenderMessage &rmsg);
		BOOL HitTest (struct HitTestMessage &hmsg);

		VOID AllocateColours (struct ColorMap *cmap);
		VOID FreeColours (struct ColorMap *cmap);

	protected:
		LONG TextRGB, LinkRGB, VLinkRGB, ALinkRGB;
		LONG TextCol, LinkCol, VLinkCol, ALinkCol, HalfshineCol, HalfshadowCol;
		struct ColorMap *CMap;
};

#endif // BODYCLASS_H
