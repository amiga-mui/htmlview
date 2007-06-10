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

#include "SuperClass.h"

#include "Layout.h"

class SuperClass *SuperClass::FindElement (ULONG tagID)
{
  return(ID == tagID ? this : NULL);
}

class AClass *SuperClass::FindAnchor (STRPTR name UNUSED)
{
  return NULL;
}

VOID SuperClass::Relayout (BOOL all)
{
  UWORD mask = (FLG_Layouted | FLG_WaitingForSize);
  if(all)
    mask |= FLG_KnowsMinMax;

  Flags &= ~mask;
}

BOOL SuperClass::Layout (struct LayoutMessage &lmsg)
{
  Top = lmsg.Y;
  Bottom = Top + lmsg.Baseline + lmsg.Bottom;

   return TRUE;
}

VOID SuperClass::AdjustPosition (LONG x UNUSED, LONG y)
{
  Top += y;
  Bottom += y;
}

BOOL SuperClass::Mark (struct MarkMessage &mmsg)
{
  if(mmsg.Y1 < Top)
    mmsg.Flags |= MarkFLG_FirstCoordTaken;
  if(mmsg.Y2 < Top)
    mmsg.Flags |= MarkFLG_SecondCoordTaken;
  if(mmsg.LastY < Top)
    mmsg.Flags |= MarkFLG_LastCoordTaken;

  return(mmsg.Flags == (MarkFLG_FirstCoordTaken | MarkFLG_SecondCoordTaken | MarkFLG_LastCoordTaken));
}

WORD SuperClass::InDomain (struct RenderMessage &rmsg)
{
  if(Top > rmsg.MaxY+rmsg.OffsetY)
  {
    return BelowLayer;
  }

  if(Bottom < rmsg.MinY+rmsg.OffsetY)
  {
    return AboveLayer;
  }

  struct Layer *layer = rmsg.RPort->Layer;
  LONG y = layer->bounds.MinY;
  LONG top = Top-rmsg.OffsetY+y;
  LONG bottom = Bottom-rmsg.OffsetY+y;

  WORD result = BelowLayer;
  for(struct ClipRect *cr = layer->ClipRect; cr; cr = cr->Next)
  {
    if(bottom < cr->bounds.MinY)
      result = AboveLayer;
    else if(top <= cr->bounds.MaxY)
    {
      return InsideLayer;
    }
  }

  return result;
}

