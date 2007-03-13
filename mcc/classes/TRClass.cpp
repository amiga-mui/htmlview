
#include "TRClass.h"
#include "TDClass.h"

#include "General.h"
#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

BOOL TRClass::TRLayout (struct LayoutMessage &lmsg)
{
	if(lmsg.Pass == 0)
	{
		FindImage(lmsg);

		Top = Bottom = lmsg.Y;
		Flags &= ~FLG_Virgin;

		ULONG X = lmsg.X, MinX = lmsg.MinX, MaxX = lmsg.MaxX, Y = lmsg.Y, MinY = lmsg.MinY;
		ULONG Indent = lmsg.Indent;
		struct CellWidth *widths = lmsg.Widths;
		ULONG *opencounts = lmsg.RowOpenCounts;
		LONG cols = lmsg.Columns;

		lmsg.MinX = lmsg.X += lmsg.Spacing;

		struct ChildsList *first = FirstChild;
		while(first)
		{
			while(*opencounts)
			{
				ULONG delta = widths->Width + lmsg.Spacing;
				lmsg.MinX += delta;
				lmsg.X += delta;

				widths++;
				*opencounts++ -= 1;
				cols--;
			}

			class TDClass *td = (class TDClass *)first->Obj;
			ULONG delta = 0, colspan = td->ColSpan;
			while(colspan--)
			{
				*opencounts++ = td->RowSpan-1;
				delta += widths->Width + lmsg.Spacing;
				widths++;
				cols--;
			}
			lmsg.Y = lmsg.MinY = Y;
			lmsg.MaxX = lmsg.MinX + delta - 1 - lmsg.Padding - lmsg.Spacing;
			lmsg.Indent = 0;
			td->TDLayout(lmsg);
			if(td->RowSpan == 1)
				Bottom = max(lmsg.Y, Bottom);
			lmsg.Heights[td->RowSpan-1] = max(lmsg.Y, lmsg.Heights[td->RowSpan-1]);

			lmsg.MinX += delta;
			lmsg.X += delta;

			first = first->Next;
		}

		if(!FirstChild)
			*lmsg.Heights = max(Top + 2*lmsg.Padding, *lmsg.Heights);

		if(*lmsg.Heights)
				lmsg.Y = *lmsg.Heights;
		else	lmsg.Y = *lmsg.Heights = Top + 2*lmsg.Padding;

		lmsg.Heights++;

		lmsg.X = X;
		lmsg.MinX = MinX;
		lmsg.MaxX = MaxX;
		lmsg.MinY = MinY;
		lmsg.Indent = Indent;
		lmsg.AddYSpace(lmsg.Spacing);

		while(cols-- > 0)
		{
			if(*opencounts)
				*opencounts -= 1;
			opencounts++;
		}
	}
	else
	{
		struct ChildsList *first = FirstChild;
		while(first)
		{
			class TDClass *td = (class TDClass *)first->Obj;
			LONG bottom = lmsg.Heights[td->RowSpan-1];
			ULONG offset = 0;

			switch(td->VAlignment)
			{
				case Align_Middle:
					offset = (bottom - td->bottom())/2;
				break;

				case Align_Bottom:
					offset = bottom - td->bottom();
				break;
			}

			if(offset)
			{
				td->setTop(td->top() - offset);
				td->AdjustPosition(0, offset);
			}
			td->setBottom(bottom-1);
			Bottom = max(td->bottom(), Bottom);

			first = first->Next;
		}
		lmsg.Heights++;

		Flags |= FLG_Layouted;
	}
}

BOOL TRClass::Mark (struct MarkMessage &mmsg)
{
	return(TreeClass::Mark(mmsg));
}

VOID TRClass::CountCells (struct CountCellsMessage &cmsg)
{
	ULONG t_columns = cmsg.OpenRows;
	struct ChildsList *first = FirstChild;
	while(first)
	{
		class TDClass *td = (class TDClass *)first->Obj;
		UWORD colspan = td->ColSpan, rowspan = td->RowSpan;

		while(colspan--)
		{
			t_columns++;
			if(rowspan > 1)
			{
				cmsg.RowSpan[cmsg.OpenRows] = rowspan;
				cmsg.OpenRows++;
			}
		}

		first = first->Next;
	}

	UWORD limit = cmsg.OpenRows;
	UWORD *src = cmsg.RowSpan, *dst = src;
	for(UWORD i = 0; i < limit; i++)
	{
		if(*dst = *src++ - 1)
				dst++;
		else	cmsg.OpenRows--;
	}

	cmsg.Rows++;
	cmsg.Columns = max(cmsg.Columns, t_columns);
}

VOID TRClass::TRMinMax (struct MinMaxMessage &mmsg)
{
	LONG cols = mmsg.Columns;
	struct ChildsList *first = FirstChild;
	while(first)
	{
		while(*mmsg.RowOpenCounts)
		{
			*mmsg.RowOpenCounts++ -= 1;
			mmsg.Widths++;
			cols--;
		}
		((class TDClass *)first->Obj)->TDMinMax(mmsg);
		cols -= ((class TDClass *)first->Obj)->ColSpan;
		first = first->Next;
	}

	while(cols-- > 0)
	{
		if(*mmsg.RowOpenCounts)
			*mmsg.RowOpenCounts -= 1;
		mmsg.RowOpenCounts++;
	}

	Flags |= FLG_KnowsMinMax;
}

VOID TRClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	LONG alignment = -1, valignment = -1;
	LONG oldalign = pmsg.CellAlignment, oldvalign = pmsg.CellVAlignment;
	struct ArgList args[] =
	{
		{ "BACKGROUND",	&Source,				ARG_URL		},
		{ "BGCOLOR",		&BackgroundRGB,	ARG_COLOUR	},

		{ "ALIGN",			&alignment,			ARG_KEYWORD, AlignKeywords },
		{ "VALIGN",			&valignment,		ARG_KEYWORD, VAlignKeywords },
		{ NULL }
	};
	ScanArgs(pmsg.Locked, args);

	pmsg.CellAlignment = alignment + 1;
	pmsg.CellVAlignment = valignment + 1  ? valignment + Align_Top : Align_Middle;
	BackFillClass::Parse(pmsg);
	pmsg.CellAlignment = oldalign;
	pmsg.CellVAlignment = oldvalign;

	Verify();
}

VOID TRClass::Verify ()
{
	struct ChildsList *prev = (struct ChildsList *)&FirstChild, *first = FirstChild;
	while(first)
	{
		if(first->Obj->group() != group_Tablecell)
		{
			if(first->Obj->id() == tag_COMMENT || first->Obj->id() == tag_Text)
			{
				struct ChildsList *next = first->Next;
				delete first->Obj;
				delete first;
				prev->Next = next;
				first = next;
			}
			else
			{
				class TDClass *td = new class TDClass();
				td->setId(tag_TD);
				td->setFlags(td->flags() | FLG_ArgumentsRead);
				td->AddChild(first->Obj);
				first->Obj = td;

				prev = first;
				first = first->Next;

				while(first && first->Obj->group() != group_Tablecell)
				{
					td->AddChild(first->Obj);
					prev->Next = first->Next;
					delete first;
					first = prev->Next;
				}
			}
		}
		else
		{
			prev = first;
			first = first->Next;
		}
	}
	RealFirstChild = FirstChild;
}

VOID TRClass::TRRender (struct RenderMessage &rmsg)
{
	LONG y1 = Top-rmsg.OffsetY, y2 = Bottom-rmsg.OffsetY;
	if(y1 <= rmsg.MaxY && y2 >= rmsg.MinY)
	{
		LONG old_col = rmsg.CellBackgroundCol;
		struct PictureFrame *old_pic = rmsg.CellPicture;
		rmsg.CellBackgroundCol = BackgroundCol;
		rmsg.CellPicture = Picture;

		BOOL target;
		if((target = rmsg.TargetObj == (class SuperClass *)this))
			rmsg.TargetObj = NULL;

		struct ChildsList *first = FirstChild;
		while(first)
		{
			((class TDClass *)first->Obj)->TDRender(rmsg);
			first = first->Next;
		}

		rmsg.CellPicture = old_pic;
		rmsg.CellBackgroundCol = old_col;

		if(target)
			rmsg.TargetObj = this;
	}
}


