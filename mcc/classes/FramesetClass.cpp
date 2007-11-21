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

#include "FramesetClass.h"
#include "HostClass.h"

#include "General.h"
#include "Layout.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

Object *FramesetClass::LookupFrame (STRPTR name, class HostClass *hclass)
{
  Object *result = NULL;
  struct ChildsList *first = FirstChild;
  while(first && !result)
  {
    result = first->Obj->LookupFrame(name, hclass);
    first = first->Next;
  }
  return(result);
}

Object *FramesetClass::FindDefaultFrame (ULONG &size)
{
  Object *result = NULL, *t_result;
  struct ChildsList *first = FirstChild;

  while(first)
  {
    if((t_result = first->Obj->FindDefaultFrame(size)))
      result = t_result;
    first = first->Next;
  }
  
  return(result);
}

Object *FramesetClass::HandleMUIEvent (struct MUIP_HandleEvent *emsg)
{
  Object *result = NULL, *t_result;
  struct ChildsList *first = FirstChild;

  while(first)
  {
    if((t_result = first->Obj->HandleMUIEvent(emsg)))
      result = t_result;
    first = first->Next;
  }
  
  return(result);
}

BOOL FramesetClass::Layout (struct LayoutMessage &lmsg)
{
  if((Flags & (FLG_AllElementsPresent | FLG_Layouted)) == FLG_AllElementsPresent)
  {
    ULONG b1 = lmsg.MinX, b2 = lmsg.MinY, b3 = lmsg.MaxX, b4 = lmsg.MaxY;
    LONG width = lmsg.MaxX - lmsg.MinX + 1, height = lmsg.MaxY - lmsg.MinY + 1;

    Left = b1, Top = b2, Width = width, Bottom = b4;
    lmsg.TopChange = min(lmsg.TopChange, Top);

    delete Columns;
    delete Rows;
    Columns = ConvertSizeList(ColumnsStr, width, ColumnCnt);
    Rows = ConvertSizeList(RowsStr, height, RowCnt);

    BOOL leftborder = lmsg.LeftBorder;
    ULONG *rows = Rows;
    struct ChildsList *first = FirstChild;
    for(UWORD r = 0; r < RowCnt; r++)
    {
      lmsg.LeftBorder = leftborder;
      lmsg.MinX = b1;
      lmsg.MaxY = lmsg.MinY + *rows - 1;
      BOOL topborder = lmsg.TopBorder;
      ULONG *cols = Columns;
      for(UWORD c = 0; c < ColumnCnt; c++)
      {
        lmsg.TopBorder = topborder;
        lmsg.MaxX = lmsg.MinX + *cols - 1;
        if(first)
        {
          first->Obj->Layout(lmsg);
          first = first->Next;
        }
        lmsg.MinX += *cols++;
      }
      lmsg.MinY += *rows++;
    }

    lmsg.MinX = b1;
    lmsg.MinY = b2;
    lmsg.MaxX = b3;
    lmsg.MaxY = b4;
    Flags |= FLG_Layouted;
  }
  else
  {
    lmsg.TopChange = min(lmsg.TopChange, MAX_HEIGHT);
  }

  return TRUE;
}

VOID FramesetClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  struct ArgList args[] =
  {
    { "COLS",         &ColumnsStr,        ARG_STRING,  NULL },
    { "ROWS",         &RowsStr,           ARG_STRING,  NULL },
    { "FRAMEBORDER",  &pmsg.FrameBorder,  ARG_BOOLEAN, NULL },  // This is wrong
    { NULL,           NULL,               0,           NULL }
  };
  ScanArgs(pmsg.Locked, args);

  TreeClass::Parse(pmsg);

  struct ChildsList *prev = (struct ChildsList *)&FirstChild, *first = FirstChild;
  while(first)
  {
    if(first->Obj->id() != tag_FRAME && first->Obj->id() != tag_FRAMESET)
    {
      prev->Next = first->Next;
      delete first->Obj;
      delete first;
      first = prev->Next;
    }
    else
    {
      prev = first;
      first = first->Next;
    }
  }

  Flags |= FLG_AllElementsPresent;
}

VOID FramesetClass::Render (struct RenderMessage &rmsg)
{
  struct RastPort *rp = rmsg.RPort;

  ULONG x, y = Top, top, bottom;
  ULONG *rows = Rows;
  for(UWORD r = 0; r < RowCnt; r++)
  {
    top = y - rmsg.OffsetY;
    bottom = top + *rows - 1;
    x = Left;
    ULONG *cols = Columns;
    for(UWORD c = 0; c < ColumnCnt-1; c++)
    {
      x += *cols++;
      SetAPen(rp, rmsg.Colours[Col_Background]);
      RectFill(rp, x-rmsg.OffsetX, top, x+5-rmsg.OffsetX, bottom);

      SetAPen(rp, rmsg.Colours[Col_Shine]);
      RectFill(rp, x+1-rmsg.OffsetX, top, x+1-rmsg.OffsetX, bottom);
      SetAPen(rp, rmsg.Colours[Col_Shadow]);
      RectFill(rp, x+4-rmsg.OffsetX, top, x+4-rmsg.OffsetX, bottom);
      x += 6;
    }

    if(r < RowCnt-1)
    {
      ULONG left = Left-rmsg.OffsetX, right = left+Width-1;
      y += *rows++;
      SetAPen(rp, rmsg.Colours[Col_Background]);
      RectFill(rp, left, y-rmsg.OffsetY, right, y+5-rmsg.OffsetY);

      SetAPen(rp, rmsg.Colours[Col_Shine]);
      RectFill(rp, left, y+1-rmsg.OffsetY, right, y+1-rmsg.OffsetY);
      SetAPen(rp, rmsg.Colours[Col_Shadow]);
      RectFill(rp, left, y+4-rmsg.OffsetY, right, y+4-rmsg.OffsetY);

      y += 6;
    }
  }
  TreeClass::Render(rmsg);
}

ULONG *FramesetClass::ConvertSizeList(STRPTR value_list, LONG total, ULONG &cnt)
{
  if(!value_list)
    value_list = (STRPTR)"*";

  LONG value, slices = 0, substract = 0;

  STRPTR t_value_list = value_list;
  ULONG values = 1;
  while(*t_value_list)
  {
    if(*t_value_list++ == ',')
      values++;
  }
  cnt = values;

  LONG *Translated = new LONG [values];
  LONG *trans = Translated;

  t_value_list = value_list;
  ULONG t_values = values;
  while(t_values--)
  {
    while(!isdigit(*t_value_list) && *t_value_list != '*')
      t_value_list++;

    STRPTR current = t_value_list;
    while(*t_value_list && isdigit(*t_value_list))
      t_value_list++;

    sscanf(current, "%ld", &value);
    if(*t_value_list != '*')
    {
      if(*t_value_list == '%')
      {
        if((value = (total * value) / 100) <= 0)
          value = 1;
      }
      *trans = value;
      substract += value;
    }
    else
    {
      slices += (*current == '*') ? 1 : value;
      t_value_list++;
    }
    trans++;
  }

  total -= substract;
  substract = 0;

  trans = Translated;
  if(slices)
  {
    t_values = values;
    while(t_values--)
    {
      while(!isdigit(*value_list) && *value_list != '*')
        value_list++;

      STRPTR current = value_list;
      while(*value_list && isdigit(*value_list))
        value_list++;

      if(*value_list == '*')
      {
        if(*current == '*')
          value =  1;
        else
          sscanf(current, "%ld", &value);

        if((value = (total * value) / slices) <= 0)
          value = 1;

        *trans = value;
        substract += value;
        value_list++;
      }
      trans++;
    }
  }

  if(total -= substract)
  {
    LONG actualwidth = 0, carry = 0, neg = total < 0 ? -1 : 1;
    for(unsigned int i = 0; i < values; i++)
      actualwidth += Translated[i];

    for(unsigned int i = 0; i < values; i++)
    {
      LONG denominator = Translated[i] * total;
      substract = denominator / actualwidth;
      carry += denominator - (substract * actualwidth);
      if(abs(carry) >= actualwidth)
      {
        carry -= neg * actualwidth;
        substract += neg;
      }
      if(!(Translated[i] += substract))
        Translated[i] = 1;
    }
  }

/*  printf("%s\n%d: ", widthstr, width);
  for(UWORD i = 0; i < values; i++)
    printf("%d, ", Translated[i]);
  printf("\n");
*/
  return((ULONG *)Translated);
}

