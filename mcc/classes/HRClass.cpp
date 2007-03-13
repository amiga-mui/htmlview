
#include "HRClass.h"

#include "General.h"
#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

BOOL HRClass::Layout (struct LayoutMessage &lmsg)
{
	lmsg.AddYSpace(8+Height);
	Top = lmsg.Y-(4+Height);
	Bottom = lmsg.Y-5;

	ULONG width = lmsg.ScrWidth();
	if(GivenWidth)
	{
		if(GivenWidth->Type == Size_Percent)
				width = (width * GivenWidth->Size) / 100;
		else	width = GivenWidth->Size;
	}

	Left = lmsg.X;
	if((ULONG)lmsg.ScrWidth() > width)
	{
		if(Alignment == Align_Center)
			Left += (lmsg.ScrWidth() - width) / 2;
		else if(Alignment == Align_Right)
			Left += lmsg.ScrWidth() - width;
	}
	else
		lmsg.Width = max(lmsg.X + width + lmsg.MarginWidth, (ULONG)lmsg.Width);

	Right = Left + width - 1;

	Flags |= FLG_Layouted;

  return TRUE;
}

VOID HRClass::AdjustPosition (LONG x, LONG y)
{
	Left += x;
	Right += x;
	SuperClass::AdjustPosition(x, y);
}

BOOL HRClass::Mark (struct MarkMessage &mmsg)
{
	BOOL result = SuperClass::Mark(mmsg);

	if(mmsg.Enabled() && mmsg.Copy)
	{
		if(!mmsg.Newline)
			mmsg.WriteLF();

		for(UWORD c = 0; c < 78; c++)
			mmsg.WriteText((STRPTR)"-", 1);

		mmsg.WriteLF();
		mmsg.Newline = TRUE;
	}

	return(result);
}

VOID HRClass::MinMax (struct MinMaxMessage &mmsg)
{
	mmsg.Newline();
	if(GivenWidth && GivenWidth->Type == Size_Pixels)
	{
		mmsg.X += GivenWidth->Size;
		mmsg.Newline();
	}
	Flags |= FLG_KnowsMinMax;
}

VOID HRClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	AttrClass::Parse(pmsg);

	BOOL noshade = FALSE;
	struct ArgList args[] =
	{
		{ "NOSHADE",	&noshade,		  ARG_SWITCH,   NULL },
		{ "WIDTH",		&GivenWidth,	ARG_VALUE,    NULL },
		{ "SIZE",		  &Height,			ARG_NUMBER,   NULL },
		{ "ALIGN",		&Alignment,		ARG_KEYWORD,  AlignKeywords },
		{ NULL,       NULL,         0,            NULL }
	};
	Alignment = Align_Center-1;
	ScanArgs(pmsg.Locked, args);
	Alignment++;
	if(noshade)
		Flags |= FLG_HR_NoShade;
	if(!Height)
		Height = 2;
}

VOID HRClass::Render (struct RenderMessage &rmsg)
{
	struct RastPort *rp = rmsg.RPort;
	SetAPen(rp, rmsg.Colours[Col_Halfshadow]);

	if(Flags & FLG_HR_NoShade || Height == 1)
	{
		RectFill(rp, Left-rmsg.OffsetX, Top-rmsg.OffsetY, Right-rmsg.OffsetX, Bottom-rmsg.OffsetY);
	}
	else
	{
		RectFill(rp, Left-rmsg.OffsetX, Top-rmsg.OffsetY, Right-1-rmsg.OffsetX, Top-rmsg.OffsetY);
		RectFill(rp, Left-rmsg.OffsetX, Top-rmsg.OffsetY, Left-rmsg.OffsetX, Bottom-rmsg.OffsetY);
		SetAPen(rp, rmsg.Colours[Col_Halfshine]);
		RectFill(rp, Left+1-rmsg.OffsetX, Bottom-rmsg.OffsetY, Right-rmsg.OffsetX, Bottom-rmsg.OffsetY);
		RectFill(rp, Right-rmsg.OffsetX, Top-rmsg.OffsetY, Right-rmsg.OffsetX, Bottom-rmsg.OffsetY);
	}
}

