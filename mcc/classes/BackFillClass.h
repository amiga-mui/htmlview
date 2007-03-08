#ifndef BACKFILLCLASS_H
#define BACKFILLCLASS_H

#include "TreeClass.h"

// forward declarations

class BackFillClass : public TreeClass
{
	public:
		BackFillClass () : TreeClass() { BackgroundRGB = -1; }
		~BackFillClass ();
		VOID Parse (REG(a2, struct ParseMessage &pmsg));

		VOID FindImage (struct LayoutMessage &lmsg);
		VOID GetImages (struct GetImagesMessage &gmsg);
		BOOL ReceiveImage (struct PictureFrame *pic);
		BOOL UpdateImage (LONG ystart, LONG ystop, LONG top, LONG bottom, BOOL last);
		BOOL FlushImage (LONG top, LONG bottom);
		BOOL ReadyForAlpha ();
		BOOL Complex ();

		VOID AllocateColours (struct ColorMap *cmap);
		VOID FreeColours (struct ColorMap *cmap);

		VOID DrawBackground (struct RenderMessage &rmsg, LONG left, LONG top, LONG width, LONG height, LONG xoffset, LONG yoffset);

	protected:
		STRPTR Source;
		struct PictureFrame *Picture;
		LONG BackgroundRGB, BackgroundCol;
};

#endif // BACKFILLCLASS_H
