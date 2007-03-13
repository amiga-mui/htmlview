
#include "FontClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"
#include "SharedData.h"

VOID FontClass::AllocateColours (struct ColorMap *cmap)
{
	FontCol = AllocPen(cmap, FontRGB);
	Flags |= FLG_AllocatedColours;
	TreeClass::AllocateColours(cmap);
}

VOID FontClass::FreeColours (struct ColorMap *cmap)
{
	if(Flags & FLG_AllocatedColours)
	{
		FreePen(cmap, FontCol);
		Flags &= ~FLG_AllocatedColours;
	}
	TreeClass::FreeColours(cmap);
}

BOOL FontClass::Layout (struct LayoutMessage &lmsg)
{
	if(!(Flags & FLG_AllocatedColours))
		AllocateColours(lmsg.Share->Scr->ViewPort.ColorMap);

	struct TextFont *oldfont = lmsg.Font;
	if(Font != Font_None)
		lmsg.Font = lmsg.Share->Fonts[Font];

	TreeClass::Layout(lmsg);

	lmsg.Font = oldfont;

  return TRUE;
}

VOID FontClass::MinMax (struct MinMaxMessage &mmsg)
{
	struct TextFont *oldfont = mmsg.Font;
	if(Font != Font_None)
		mmsg.Font = mmsg.Fonts[Font];

	TreeClass::MinMax(mmsg);

	mmsg.Font = oldfont;
}

VOID FontClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	STRPTR font = NULL;
	struct ArgList args[] =
	{
		{ "COLOR",	&FontRGB,	ARG_COLOUR, NULL },
		{ "SIZE",	  &font,		ARG_STRING, NULL },
		{ NULL,     NULL,     0,          NULL }
	};

	ScanArgs(pmsg.Locked, args);

	LONG size = Font_None;
	if(font && sscanf(font, "%*[+-]%ld", &size) == 2)
	{
		switch(*font)
		{
			case '+':
				size = Font_H4 - ((size > 3) ? 3 : size);
			break;

			case '-':
				size = Font_H4 + ((size > 2) ? 2 : size);
			break;

			default:
				size = Font_Fixed - ((size > 6) ? 6 : size);
			break;
		}
	}
//	if(font) printf("Converted: %d (%s)\n", size, font);
	delete font;
	Font = size;

	TreeClass::Parse(pmsg);
}

VOID FontClass::Render (struct RenderMessage &rmsg)
{
	LONG oldcol = rmsg.Colours[Col_Text];
	SetCol(rmsg.Colours[Col_Text], FontCol);
	TreeClass::Render(rmsg);
	rmsg.Colours[Col_Text] = oldcol;
}

