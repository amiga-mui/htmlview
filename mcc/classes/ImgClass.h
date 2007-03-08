#ifndef IMGCLASS_H
#define IMGCLASS_H

#include "AttrClass.h"

// forward declarations

class ImgClass : public AttrClass
{
	public:
		ImgClass () : AttrClass() { ; }
		~ImgClass ();
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID Relayout (BOOL all);
		VOID AdjustPosition (LONG x, LONG y);
		VOID GetImages (struct GetImagesMessage &gmsg);
		VOID Render (struct RenderMessage &rmsg);
		VOID FreeColours (struct ColorMap *cmap);
		BOOL HitTest (struct HitTestMessage &hmsg);

		BOOL ReceiveImage (struct PictureFrame *pic);
		BOOL UpdateImage (LONG ystart, LONG ystop, LONG top, LONG bottom, BOOL last);
		BOOL FlushImage (LONG top, LONG bottom);

		LONG Width (LONG width = 80, struct LayoutMessage *lmsg = NULL);
		LONG Height (LONG height = 20, struct LayoutMessage *lmsg = NULL);

//	protected:
		LONG Left, Baseline;
		struct ArgSize *GivenWidth, *GivenHeight;
		LONG YStart, YStop;
		struct BitMap *BlendBitMap;
		struct PictureFrame *Picture;
		STRPTR AltText, Source, Name;
		ULONG ImgBorder;
		ULONG Alignment;
		STRPTR Map;
		class MapClass *MapObj;
		ULONG HSpace, VSpace;

  protected:
    BOOL TestPixel(UBYTE *line, LONG x);
};

#endif // IMGCLASS_H
