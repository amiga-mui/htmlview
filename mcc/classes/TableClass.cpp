
#include "TableClass.h"
#include "TRClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"
#include "SharedData.h"

static LONG RelativeScale(struct CellWidth *cell)
{
  return(cell->Relative);
}

static LONG NormalScale(struct CellWidth *cell)
{
  return(cell->Percent ? 0 : cell->Max - cell->Min);
}

static LONG LeftOverScale(struct CellWidth *cell)
{
  return(cell->Fixed ? 0 : cell->Max);
}

class SuperClass *TableClass::FindElement (ULONG tagID)
{
	return((tagID == tag_TD || tagID == tag_TR) ? NULL : TreeClass::FindElement(tagID));
}

VOID TableClass::Relayout (BOOL all)
{
	if(all)
	{
		delete Widths;
		delete Heights;
		delete RowOpenCounts;
		Widths = NULL;
		Heights = RowOpenCounts = NULL;
	}
	TreeClass::Relayout(all);
}

VOID TableClass::Spread (LONG scale, LONG width, LONG (*get_width) (struct CellWidth *))
{
	LONG carry = 0, cell_width, add, denominator;
	for(UWORD i = 0; i < Columns; i++)
	{
		if((cell_width = get_width(&Widths[i])))
		{
			denominator = cell_width * scale;
			add = denominator / width;
			carry += denominator - (add * width);
			if(carry >= width)
			{
				carry -= width;
				add++;
			}
			Widths[i].Width += add;
		}
	}
}

BOOL TableClass::Layout (struct LayoutMessage &lmsg)
{
	if(Flags & FLG_AllElementsPresent)
	{
		FindImage(lmsg);
		if(!(Flags & FLG_AllocatedColours))
			AllocateColours(lmsg.Share->Scr->ViewPort.ColorMap);

/*		if(lmsg.Y != lmsg.MinY || !lmsg.IsAtNewline())
			lmsg.AddYSpace(4);
*/		lmsg.EnsureNewline();

		Top = lmsg.Y;
		Left = lmsg.X;
		lmsg.TopChange = min(Top, lmsg.TopChange);

		if(!(Flags & FLG_KnowsMinMax))
		{
			struct MinMaxMessage mmsg(lmsg.Share->Fonts, &lmsg);
			mmsg.Reset();
			MinMax(mmsg);
		}

		ULONG scr_width = lmsg.ScrWidth() - (Columns+1)*Spacing - 2*BorderSize;
		if(GivenWidth)
		{
			if(GivenWidth->Type == Size_Percent)
			   Width = max(Min, ((lmsg.ScrWidth() * GivenWidth->Size) / 100) - (Columns+1)*Spacing - 2*BorderSize);
			else
            Width = max(GivenWidth->Size-(Columns+1)*Spacing-2*BorderSize, Min);
		}
		else 
         Width = (Min <= scr_width) ? min(scr_width, Max) : Min;

		/* Set all cells to their min or max width */
		LONG scale = Width, table_delta = 0, relative = 0;
		for(ULONG i = 0; i < Columns; i++)
		{
			if(!Widths[i].Percent)
			{
				LONG cellwidth = Width >= Max ? Widths[i].Max : Widths[i].Min;
				table_delta += Widths[i].Max - Widths[i].Min;
				scale -= cellwidth;
				relative += Widths[i].Relative;
				Widths[i].Width = cellwidth;
			}
		}

		/* Set all cells with a width given in percent */
		for(ULONG i = 0; i < Columns; i++)
		{
			if(Widths[i].Percent)
			{
				ULONG cellwidth = max(Widths[i].Min, min(scale, (Width * Widths[i].Percent) / 100));
				Widths[i].Width = cellwidth;
				scale -= cellwidth;
			}
		}

		if(scale > 0)
		{
			if(relative)
			{
				Spread(scale, relative, RelativeScale);
			}
			else if(table_delta)
			{
				Spread(scale, table_delta, NormalScale);
			}
			else
			{
				LONG width = Max;
				for(UWORD i = 0; i < Columns; i++)
				{
					if(Widths[i].Fixed)
						width -= Widths[i].Max;
				}

				if(width)
						Spread(scale, width, LeftOverScale);
				else	Width -= scale;
			}
		}
		else
		{
			Width -= scale;
		}

		struct CellWidth *oldwidths = lmsg.Widths;
		ULONG oldspace = lmsg.Spacing, oldpad = lmsg.Padding;
		struct TextFont *oldfont = lmsg.Font;

		ULONG oldcols = lmsg.Columns, oldimageleftindent = lmsg.ImageLeftIndent, oldimagerightindent = lmsg.ImageRightIndent;
		lmsg.ImageLeftIndent = lmsg.ImageRightIndent = 0;
		struct FloadingImage *fleft = lmsg.FLeft, *fright = lmsg.FRight;
		lmsg.FLeft = lmsg.FRight = NULL;

		lmsg.Widths = Widths;
		lmsg.Spacing = Spacing;
		lmsg.Padding = Padding + (BorderSize ? 1 : 0);
		lmsg.Columns = Columns;

		ULONG oldminx = lmsg.MinX;
		lmsg.MinX = lmsg.X += BorderSize;

		lmsg.AddYSpace(Spacing+BorderSize);
		Flags &= ~FLG_Virgin;

		ULONG *oldopencounts = lmsg.RowOpenCounts;
		lmsg.RowOpenCounts = (ULONG *)memset(RowOpenCounts, 0, Columns * sizeof(ULONG));

		ULONG *oldheights = lmsg.Heights;
		ULONG pass = lmsg.Pass;
		lmsg.Pass = 0;
		lmsg.Heights = (ULONG *)memset(Heights, 0, Rows*sizeof(ULONG));

		ULONG realrows = 0;
		struct ChildsList *first = FirstChild;
		while(first)
		{
			((class TRClass *)first->Obj)->TRLayout(lmsg);
			first = first->Next;
			realrows++;
		}

		/* Should some cell set a rowspan that ends outside the table, then we enlarge all height entries */
		while(realrows < Rows)
		{
			Heights[realrows] = max(Heights[realrows], Heights[realrows-1]);
			realrows++;
		}

		Bottom = Rows ? Heights[Rows-1] : lmsg.Y;
		lmsg.Y = Bottom + Spacing;

		lmsg.Pass = 1;
		lmsg.Heights = Heights;

		first = FirstChild;
		while(first)
		{
			((class TRClass *)first->Obj)->TRLayout(lmsg);
			first = first->Next;
		}

		lmsg.Pass = pass;
		lmsg.Heights = oldheights;

		lmsg.RowOpenCounts = oldopencounts;

		lmsg.MinX = oldminx;
		lmsg.AddYSpace(BorderSize);

		lmsg.Widths = oldwidths;
		lmsg.Columns = oldcols;
		lmsg.Spacing = oldspace;
		lmsg.Padding = oldpad;

		lmsg.FLeft = fleft;
		lmsg.FRight = fright;
		lmsg.ImageLeftIndent = oldimageleftindent;
		lmsg.ImageRightIndent = oldimagerightindent;
		lmsg.X += oldimageleftindent;

		lmsg.Font = oldfont;

		lmsg.Width = max(lmsg.Width, lmsg.MinX+Width+((Columns+1)*Spacing)+2*BorderSize+lmsg.MarginWidth+lmsg.ImageRightIndent);

		Bottom = lmsg.Y-1;
//		lmsg.AddYSpace(4);

		LONG width = Width+((Columns+1)*Spacing)+2*BorderSize;
		LONG delta = lmsg.ScrWidth() - width;
		UBYTE oldalign = lmsg.Align;
		if(delta > 0)
		{
			LONG offset = 0;

			if(Alignment == Align_Left || Alignment == Align_Right)
			{
				LONG width = Width + (Columns+1)*Spacing + 2*BorderSize;
				struct FloadingImage *img = new struct FloadingImage(Top, Left, width, (Bottom-Top)+1, this, lmsg.Parent);
				LONG left = lmsg.AddImage(img, Alignment == Align_Right);

				offset = left - Left;

				lmsg.Y = Top;
				lmsg.Baseline = lmsg.Bottom = 0;
			}
			else if(lmsg.Align == Align_Center || Alignment == Align_Center)
			{
				offset = delta/2;
			}
			else if(lmsg.Align == Align_Right)
			{
				offset = delta;
			}

			if(offset)
				AdjustPosition(offset, 0);
		}

		lmsg.Align = oldalign;

		if(Alignment != Align_Left && Alignment != Align_Right)
			lmsg.CheckFloatingObjects();

		Flags |= FLG_Layouted;
	}
	else
	{
		lmsg.TopChange = min(lmsg.TopChange, MAX_HEIGHT);
	}

   return TRUE;
}

VOID TableClass::AdjustPosition (LONG x, LONG y)
{
	Left += x;
	TreeClass::AdjustPosition(x, y);
}

BOOL TableClass::Mark (struct MarkMessage &mmsg)
{
	BOOL result = FALSE;

	LONG top = Top + BorderSize, bottom = Bottom - BorderSize;
	LONG left = Left + BorderSize, right = Left+BorderSize+Width-1+((Columns+1)*Spacing);
	struct TruncArgs args(left, top, right, bottom, mmsg.Flags);

	switch(Alignment)
	{
		case Align_Right:
		{
			const ULONG len = 8*sizeof(ULONG);
			UBYTE backup[len];
			memcpy(backup, &mmsg.X1, len);

			args.TablePreRightTruncCoords(mmsg.X1, mmsg.Y1);
			args.TablePreRightTruncCoords(mmsg.X2, mmsg.Y2);
			args.TablePreRightTruncCoords(mmsg.LastX, mmsg.LastY);

			mmsg.TableSpace = Spacing;
			TreeClass::Mark(mmsg);

			memcpy(&mmsg.X1, backup, len);

			args.TablePostRightTruncCoords(mmsg.X1, mmsg.Y1);
			args.TablePostRightTruncCoords(mmsg.X2, mmsg.Y2);
			args.TablePostRightTruncCoords(mmsg.LastX, mmsg.LastY);
		}
		break;

		case Align_Left:
		{
			UWORD flags = mmsg.Flags;
			LONG tablespace = mmsg.TableSpace;
			mmsg.TableSpace = Spacing;
			result = TreeClass::Mark(mmsg);
			mmsg.TableSpace = tablespace;
			mmsg.Flags = flags;

			args.TableLeftTruncCoords(mmsg.X1, mmsg.Y1, MarkFLG_FirstCoordTaken);
			args.TableLeftTruncCoords(mmsg.X2, mmsg.Y2, MarkFLG_SecondCoordTaken);
			args.TableLeftTruncCoords(mmsg.LastX, mmsg.LastY, MarkFLG_LastCoordTaken);

			result = mmsg.Flags == (MarkFLG_FirstCoordTaken | MarkFLG_SecondCoordTaken | MarkFLG_LastCoordTaken);
		}
		break;

		default:
		{
			const ULONG len = 8*sizeof(ULONG);
			UBYTE backup[len];
			memcpy(backup, &mmsg.X1, len);

			mmsg.Flags = 0;
			mmsg.TableSpace = Spacing;

			args.TableTruncCoords(mmsg.X1, mmsg.Y1, MarkFLG_FirstCoordTaken);
			args.TableTruncCoords(mmsg.X2, mmsg.Y2, MarkFLG_SecondCoordTaken);
			args.TableTruncCoords(mmsg.LastX, mmsg.LastY, MarkFLG_LastCoordTaken);

			TreeClass::Mark(mmsg);
			memcpy(&mmsg.X1, backup, len);
		}
		break;
	}
	return(result);
}

VOID TableClass::MinMax (struct MinMaxMessage &mmsg)
{
	struct CountCellsMessage *cmsg = new struct CountCellsMessage;
	struct ChildsList *first = FirstChild;
	while(first)
	{
		((class TRClass *)first->Obj)->CountCells(*cmsg);
		first = first->Next;
	}
	Columns = cmsg->Columns;
	Rows = cmsg->Rows;
	ULONG extra = 0;
	for(UWORD c = 0; c < cmsg->OpenRows; c++)
		extra = max(extra, cmsg->RowSpan[c]);
	Rows += extra;
	delete cmsg;

	Widths = new struct CellWidth [Columns];
	Heights = new ULONG [Rows];
	RowOpenCounts = new ULONG [Columns];

	struct MinMaxMessage t_mmsg(mmsg.Fonts, mmsg.LMsg, Widths);
	t_mmsg.Padding = Padding + (BorderSize ? 1 : 0);
	t_mmsg.Spacing = Spacing;
	t_mmsg.Columns = Columns;

	t_mmsg.Pass = 0;
	first = FirstChild;
	while(first)
	{
		t_mmsg.Reset();
		t_mmsg.Widths = Widths;
		t_mmsg.RowOpenCounts = RowOpenCounts;
		((class TRClass *)first->Obj)->TRMinMax(t_mmsg);
		first = first->Next;
	}

	t_mmsg.Pass = 1;
	first = FirstChild;
	memset(RowOpenCounts, 0, Columns * sizeof(ULONG));
	while(first)
	{
		t_mmsg.Widths = Widths;
		t_mmsg.RowOpenCounts = RowOpenCounts;
		((class TRClass *)first->Obj)->TRMinMax(t_mmsg);
		first = first->Next;
	}

	Min = Max = 0;
	for(UWORD i = 0; i < Columns; i++)
	{
		Min += Widths[i].Min;
		Max += Widths[i].Max;
	}

	mmsg.Newline();
	mmsg.Min = max(mmsg.Indent + Min + (Columns+1)*Spacing+2*BorderSize, mmsg.Min);
	mmsg.Max = max(mmsg.Indent + Max + (Columns+1)*Spacing+2*BorderSize, mmsg.Max);

	if(GivenWidth && GivenWidth->Type == Size_Pixels)
		mmsg.Min = mmsg.Max = max(GivenWidth->Size, mmsg.Min);

	Flags |= FLG_KnowsMinMax;
}

VOID TableClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	APTR handle; UBYTE group = pmsg.OpenGroups[group_Tablecell];
	pmsg.OpenGroups[group_Tablecell] = 0;
	if((handle = Backup(pmsg, 11, tag_DD, tag_DL, tag_DT, tag_LI, tag_P, tag_TD, tag_TH, tag_TR, tag_FONT, tag_CENTER, tag_B)))
	{
		struct ArgList args[] =
		{
			{ "BACKGROUND",	&Source,				ARG_URL,       NULL	},
			{ "BGCOLOR",		&BackgroundRGB,	ARG_COLOUR,    NULL	},

			{ "BORDER",			&BorderSize,		ARG_NUMBER,    NULL	},
			{ "WIDTH",			&GivenWidth,		ARG_VALUE,     NULL	},
			{ "CELLSPACING",	&Spacing,			ARG_NUMBER,    NULL	},
			{ "CELLPADDING",	&Padding,			ARG_NUMBER,    NULL	},
			{ "ALIGN",			&Alignment,			ARG_KEYWORD,   AlignKeywords },
			{ NULL,           NULL,             0,             NULL  }
		};
		Spacing = Padding = 2;
		Alignment = -1;
		ScanArgs(pmsg.Locked, args);
		Alignment++;

/*		UBYTE *old_opens = new UBYTE [tag_NumberOf + group_NumberOf];
		memcpy(old_opens, pmsg.OpenCounts, tag_NumberOf + group_NumberOf);
		memset(old_opens, 0, tag_NumberOf + group_NumberOf);
*/
		UBYTE old_align = pmsg.CellAlignment;
		UBYTE old_valign = pmsg.CellVAlignment;
		pmsg.CellAlignment = 0;
		pmsg.CellVAlignment = Align_Middle;
		BackFillClass::Parse(pmsg);
		pmsg.CellAlignment = old_align;
		pmsg.CellVAlignment = old_valign;

/*		memcpy(pmsg.OpenCounts, old_opens, tag_NumberOf + group_NumberOf);
		delete old_opens;
*/		Restore(pmsg.OpenCounts, 8, handle);

		struct ChildsList *prev = (struct ChildsList *)&FirstChild, *first = FirstChild;
		while(first)
		{
			if(first->Obj->id() != tag_TR)
			{
				if(first->Obj->id() == tag_COMMENT || first->Obj->id() == tag_Text)
				{
					prev->Next = first->Next;
					delete first->Obj;
					delete first;
					first = prev->Next;
				}
				else
				{
					class TRClass *tr = new class TRClass();
					tr->setId(tag_TR);
					tr->setFlags(tr->flags() | FLG_ArgumentsRead);
					tr->AddChild(first->Obj);
					tr->Verify();
					first->Obj = tr;

					prev = first;
					first = first->Next;

					while(first && first->Obj->group() == group_Tablecell)
					{
						tr->AddChild(first->Obj);
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
	pmsg.OpenGroups[group_Tablecell] = group;
}

VOID TableClass::Render (struct RenderMessage &rmsg)
{
	if(Flags & FLG_Layouted)
	{
		struct RastPort *rp = rmsg.RPort;
		BOOL oldborder = rmsg.TableBorder, target;

		LONG old_backgroundcol = rmsg.Colours[Col_Background];
		class SuperClass *oldbackground = rmsg.BackgroundObj;
		if(BackgroundRGB != -1 || Picture)
		{
			SetCol(rmsg.Colours[Col_Background], BackgroundCol);
			rmsg.BackgroundObj = this;
		}

		if((target = (rmsg.TargetObj == (class SuperClass *)this)))
			rmsg.TargetObj = NULL;

		if(!rmsg.TargetObj)
		{
			if(rmsg.BackgroundObj == (class BackFillClass *)this)
				DrawBackground(rmsg, Left+BorderSize-rmsg.OffsetX, Top+BorderSize-rmsg.OffsetY, Left+BorderSize+Width-1+((Columns+1)*Spacing)-rmsg.OffsetX, Bottom-BorderSize-rmsg.OffsetY, 0, 0);

			if(BorderSize)
			{
				ULONG width = Width+((Columns+1)*Spacing)+2*BorderSize;
				LONG	x1 = Left-rmsg.OffsetX, x2 = Left+width-rmsg.OffsetX-1,
						y1 = Top-rmsg.OffsetY, y2 = Bottom-rmsg.OffsetY;

				SetAPen(rp, rmsg.Colours[Col_Halfshadow]);
				RectFill(rp, x1+BorderSize, y2-BorderSize+1, x2, y2);
				RectFill(rp, x2-BorderSize+1, y1+BorderSize, x2, y2);
				SetAPen(rp, rmsg.Colours[Col_Halfshine]);
				RectFill(rp, x1, y1, x2-BorderSize, y1+BorderSize-1);
				RectFill(rp, x1, y1+BorderSize, x1+BorderSize-1, y2-BorderSize);

				DrawCorner(rp, x1, y2-BorderSize+1, BorderSize, rmsg.Colours[Col_Halfshine], rmsg.Colours[Col_Halfshadow]);
				DrawCorner(rp, x2-BorderSize+1, y1, BorderSize, rmsg.Colours[Col_Halfshine], rmsg.Colours[Col_Halfshadow]);
			}
		}

		rmsg.TableBorder = BorderSize;

		struct ChildsList *first = FirstChild;
		while(first)
		{
			((class TRClass *)first->Obj)->TRRender(rmsg);
			first = first->Next;
		}

		rmsg.TableBorder = oldborder;
		rmsg.BackgroundObj = (class BackFillClass *)oldbackground;
		rmsg.Colours[Col_Background] = old_backgroundcol;

		if(target)
			rmsg.TargetObj = this;
	}
}

VOID TableClass::DrawCorner(struct RastPort *rport, LONG x, LONG y, UWORD width, UBYTE col1, UBYTE col2)
{
	LONG y2 = y+width-1;
	while(width--)
	{
		SetAPen(rport, col1);
		RectFill(rport, x, y, x+width, y);
		y++;
		if(y2 >= y)
		{
			SetAPen(rport, col2);
			RectFill(rport, x+width, y, x+width, y2);
		}
	}
}

