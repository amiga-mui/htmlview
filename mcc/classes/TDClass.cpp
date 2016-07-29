/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2007 by HTMLview.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 HTMLview class Support Site:  http://www.sf.net/projects/htmlview-mcc/

 $Id$

***************************************************************************/

#include "TDClass.h"

#include "Layout.h"
#include "Mark.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"
#include "SharedData.h"

BOOL TDClass::TDLayout (struct LayoutMessage &lmsg)
{
  FindImage(lmsg);

  Top = lmsg.Y;
  lmsg.AddYSpace(lmsg.Padding);

  lmsg.MinY = lmsg.Y; // This indicates that no additional linebreaks should be placed infront of P-tags etc.
  lmsg.Font = lmsg.Share->Fonts[Font_Normal]; // Reset the font (actually, everything should be reset...

  Left = lmsg.X;
  Width = lmsg.ScrWidth() + lmsg.Padding;
  lmsg.MinX = lmsg.X += lmsg.Padding;

  UBYTE oldalign = lmsg.Align;
  lmsg.Align = Alignment;

  Flags &= ~FLG_Virgin;
  TreeClass::Layout(lmsg);
  lmsg.EnsureNewline();

  lmsg.FlushImages(Flush_All);
  lmsg.AddYSpace(lmsg.Padding);
  Bottom = lmsg.Y;
  lmsg.Y = max(lmsg.Y, (LONG)(Top+Height));
  lmsg.MinX = lmsg.X -= lmsg.Padding;
  lmsg.Align = oldalign;

   return TRUE;
}

VOID TDClass::AdjustPosition (LONG x, LONG y)
{
  Left += x;
  TreeClass::AdjustPosition(x, y);
}

BOOL TDClass::Mark (struct MarkMessage &mmsg)
{
  const ULONG len = 6*sizeof(ULONG) + sizeof(UWORD);
  UBYTE backup[len];
  memcpy(backup, &mmsg.X1, len);

  struct TruncArgs args(Left-mmsg.TableSpace, Top-mmsg.TableSpace, Left+Width-1, Bottom, mmsg.Flags);
  args.TDTruncCoords(mmsg.X1, mmsg.Y1, MarkFLG_FirstCoordTaken);
  args.TDTruncCoords(mmsg.X2, mmsg.Y2, MarkFLG_SecondCoordTaken);
  args.TDTruncCoords(mmsg.LastX, mmsg.LastY, MarkFLG_LastCoordTaken);

  mmsg.WriteText("\t", 1);
  Flags &= ~FLG_Newline;
  TreeClass::Mark(mmsg);
  Flags |= FLG_Newline;

  memcpy(&mmsg.X1, backup, len);

  if(args.Inside(mmsg.X1, mmsg.Y1))
    mmsg.Flags |= MarkFLG_FirstCoordTaken;
  if(args.Inside(mmsg.X2, mmsg.Y2))
    mmsg.Flags |= MarkFLG_SecondCoordTaken;
  if(args.Inside(mmsg.LastX, mmsg.LastY))
    mmsg.Flags |= MarkFLG_LastCoordTaken;

  return(FALSE);
}

VOID TDClass::TDMinMax (struct MinMaxMessage &mmsg)
{
  switch(mmsg.Pass)
  {
    case 0:
    {
      Min = Max = 0;
      mmsg.Reset();
      TreeClass::MinMax(mmsg);

      Min = mmsg.Min + 2*mmsg.Padding;
      if(GivenWidth && GivenWidth->Type == Size_Pixels)
      {
        Min = Max = max(GivenWidth->Size, Min);
        mmsg.Widths->Fixed = TRUE;
      }
      else
      {
        Max = mmsg.Max + 2*mmsg.Padding;
      }

      if(Flags & FLG_TD_NoWrap)
        Min = Max;

      if(ColSpan == 1)
      {
        mmsg.Widths->Min = max(Min, mmsg.Widths->Min);
        mmsg.Widths->Max = max(Max, mmsg.Widths->Max);
        if(GivenWidth)
        {
          if(GivenWidth->Type == Size_Percent)
          {
            mmsg.Widths->Percent = GivenWidth->Size;
            mmsg.Widths->Fixed = TRUE;
          }
          else if(GivenWidth->Type == Size_Relative)
          {
            mmsg.Widths->Relative = GivenWidth->Size;
          }
        }
      }
    }
    break;

    case 1:
    {
      if(ColSpan > 1)
      {
        LONG relative = 0, percent = 0;
        if(GivenWidth)
        {
          if(GivenWidth->Type == Size_Percent)
            percent = GivenWidth->Size;
          else if(GivenWidth->Type == Size_Relative)
            relative = GivenWidth->Size;
        }

        ULONG c_min = 0, c_max = 0, c_ignore = 0;
        for(ULONG i = 0; i < ColSpan; i++)
        {
          c_min += mmsg.Widths[i].Min;
          c_max += mmsg.Widths[i].Max;

          if(relative && !mmsg.Widths[i].Relative)
            mmsg.Widths[i].Relative = relative;

          if(mmsg.Widths[i].Percent)
          {
            percent -= mmsg.Widths[i].Percent;
            c_ignore += mmsg.Widths[i].Max;
          }
        }

        if(percent > 0)
        {
          LONG carry = 0, denominator, c_percent;
          for(ULONG i = 0; i < ColSpan; i++)
          {
            LONG divider = c_max - c_ignore;
            if(!mmsg.Widths[i].Percent && divider)
            {
              denominator = mmsg.Widths[i].Max * percent;
              c_percent = denominator / divider;
              carry += denominator - (c_percent * divider);
              if(carry >= divider)
              {
                carry -= divider;
                c_percent++;
              }
              mmsg.Widths[i].Percent = c_percent;
              mmsg.Widths[i].Fixed = TRUE;
            }
          }
        }

        if(c_min < Min)
        {
          UWORD scale = Min-c_min, row_delta = c_max-c_min;
          if(row_delta)
          {
            for(ULONG i = 0; i < ColSpan; i++)
            {
              UWORD cell_delta = mmsg.Widths[i].Max - mmsg.Widths[i].Min;
              ULONG width = (cell_delta * scale) / row_delta;
              mmsg.Widths[i].Min += width;
              mmsg.Widths[i].Max = max(mmsg.Widths[i].Min, mmsg.Widths[i].Max);
            }
          }
          else
          {
            UWORD max_scale = Max-c_max;
            for(ULONG i = 0; i < ColSpan; i++)
            {
              mmsg.Widths[i].Min += scale/ColSpan;
              mmsg.Widths[i].Max += max_scale/ColSpan;
            }
          }
        }
      }
    }
    break;
  }
  mmsg.Widths += ColSpan;
  for(UWORD i = 0; i < ColSpan; i++)
    *mmsg.RowOpenCounts++ = RowSpan-1;
}

VOID TDClass::Parse(struct ParseMessage &pmsg)
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  BOOL nowrap = FALSE;
  struct ArgList args[] =
  {
    { "BACKGROUND", &Source,        ARG_URL,          NULL  },
    { "BGCOLOR",    &BackgroundRGB, ARG_COLOUR,       NULL  },

    { "NOWRAP",     &nowrap,        ARG_SWITCH,       NULL  },
    { "WIDTH",      &GivenWidth,    ARG_MULTIVALUE,   NULL  },
    { "HEIGHT",     &Height,        ARG_NUMBER,       NULL  },
    { "COLSPAN",    &ColSpan,     ARG_NUMBER,       NULL  },
    { "ROWSPAN",    &RowSpan,     ARG_NUMBER,       NULL  },
    { "ALIGN",      &Alignment,     ARG_KEYWORD,      AlignKeywords },
    { "VALIGN",     &VAlignment,    ARG_KEYWORD,      VAlignKeywords },
    { NULL,           NULL,             0,                NULL  }
  };
  Alignment = -1;
  VAlignment = -1;
  ScanArgs(pmsg.Locked, args);
  Alignment++;
  if(!Alignment && pmsg.CellAlignment)
    Alignment = pmsg.CellAlignment;

  if(VAlignment < 0)
      VAlignment = pmsg.CellVAlignment;
  else  VAlignment += Align_Top;

  if(nowrap)
    Flags |= FLG_TD_NoWrap;

  if(!ColSpan)
    ColSpan = 1;
  if(!RowSpan)
    RowSpan = 1;

  pmsg.OpenCounts[tag_TR]++;
  BackFillClass::Parse(pmsg);
  pmsg.OpenCounts[tag_TR]--;
}

VOID TDClass::TDRender (struct RenderMessage &rmsg)
{
  LONG y1 = Top-rmsg.OffsetY, y2 = Bottom-rmsg.OffsetY;
  if(y1 <= rmsg.MaxY && y2 >= rmsg.MinY)
  {
    class SuperClass *oldbackground = rmsg.BackgroundObj;
    LONG old_backgroundcol = rmsg.Colours[Col_Background];
    LONG bgcolour = (BackgroundCol != -1) ? BackgroundCol : rmsg.CellBackgroundCol;
    struct PictureFrame *old_pic = Picture;
    if(!Picture)
      Picture = rmsg.CellPicture;
    if(bgcolour != -1 || Picture)
    {
      SetCol(rmsg.Colours[Col_Background], bgcolour);
      rmsg.BackgroundObj = this;
    }

    BOOL target;
    if((target = (rmsg.TargetObj == (class SuperClass *)this)))
      rmsg.TargetObj = NULL;

    if(!rmsg.TargetObj)
    {
      LONG x1 = Left-rmsg.OffsetX, x2 = Left+Width-rmsg.OffsetX-1;

      if(rmsg.BackgroundObj == (class BackFillClass *)this)
        DrawBackground(rmsg, x1, y1, x2, y2, 0, 0);

      if(rmsg.TableBorder)// && FirstChild)
      {
        struct RastPort *rp = rmsg.RPort;

        SetAPen(rp, rmsg.Colours[Col_Halfshine]);
        RectFill(rp, x1, y2, x2, y2);
        RectFill(rp, x2, y1, x2, y2);
        SetAPen(rp, rmsg.Colours[Col_Halfshadow]);
        RectFill(rp, x1, y1, x2, y1);
        RectFill(rp, x1, y1, x1, y2);
      }
    }
    TreeClass::Render(rmsg);
    rmsg.BackgroundObj = (class BackFillClass *)oldbackground;
    rmsg.Colours[Col_Background] = old_backgroundcol;
    Picture = old_pic;

    if(target)
      rmsg.TargetObj = this;
  }
}

