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

#include "FrameClass.h"
#include "HostClass.h"

#include "ParseMessage.h"
#include "private.h"
#include "ScanArgs.h"
#include "ScrollGroup.h"
#include "SharedData.h"

VOID FrameClass::AppendGadget (struct AppendGadgetMessage &amsg)
{
  if(!HTMLview)
  {
    ScrollGroup = (Object *)NewObject(ScrollGroupClass->mcc_Class, NULL,
      MUIA_ScrollGroup_Scrolling, Scrolling,
      MUIA_ScrollGroup_Smooth, amsg.Data->Share->Flags & FLG_SmoothPageScroll,
      MUIA_ScrollGroup_HTMLview, HTMLview = (Object *)NewObject(OCLASS(amsg.Parent), NULL,
        MUIA_HTMLview_SharedData, amsg.Data->Share,
        MUIA_HTMLview_LoadHook, amsg.Data->LoadHook,
        MUIA_HTMLview_FrameName, Name,
        MUIA_HTMLview_MarginWidth, MarginWidth,
        MUIA_HTMLview_MarginHeight, MarginHeight,
        MUIA_HTMLview_DiscreteInput, amsg.Data->Flags & FLG_DiscreteInput,
        End,
      End;

    if(!ScrollGroup)
      ScrollGroup = HTMLview = RectangleObject, MUIA_Background, MUII_TextBack, End;

    if(ScrollGroup)
      DoMethod(amsg.Parent, OM_ADDMEMBER, ScrollGroup);
  }
}

Object *FrameClass::LookupFrame (STRPTR name, class HostClass *hclass)
{
  Object *result;
  if((result = HTMLview))
  {
    if((Name && !stricmp(Name, name)) || (result = (Object *)DoMethod(HTMLview, MUIM_HTMLview_LookupFrame, name)))
      hclass->DefaultFrame = HTMLview;
  }
  return(result);
}

Object *FrameClass::FindDefaultFrame (ULONG &size)
{
  Object *result = NULL;
  if(HTMLview)
  {
    ULONG this_size = _width(HTMLview) * _height(HTMLview);
    if(this_size >= size)
    {
      size = this_size;
      result = HTMLview;
    }
  }
  return(result);
}

Object *FrameClass::HandleMUIEvent (struct MUIP_HandleEvent *emsg)
{
  Object *result = NULL;
  if(HTMLview)
  {
    DoMethod(HTMLview, MUIM_HTMLview_HandleEvent, emsg->imsg, emsg->muikey);
    if(emsg->imsg->Class == IDCMP_MOUSEBUTTONS && emsg->imsg->Code == IECODE_LBUTTON)
    {
      LONG  x = emsg->imsg->MouseX - _left(HTMLview),
          y = emsg->imsg->MouseY - _top(HTMLview);

      if(x >= 0 && y >= 0 && x < _width(HTMLview) && y < _height(HTMLview))
        result = HTMLview;
    }
  }
  return(result);
}

BOOL FrameClass::HitTest (struct HitTestMessage &hmsg)
{
  BOOL result = FALSE;

  LONG x = hmsg.X, y = hmsg.Y;
  if(y >= Top && y <= Bottom && x >= Left && x <= Right)
  {
    hmsg.X -= Left;
    hmsg.Y -= Top;
    hmsg.Frame = (char *)xget(HTMLview, MUIA_HTMLview_URL);
    hmsg.FrameObj = HTMLview;
    result = (BOOL)DoMethod(HTMLview, MUIM_HTMLview_HitTest, &hmsg);
    hmsg.X += Left;
    hmsg.Y += Top;
  }
  return(result);
}

BOOL FrameClass::Layout (struct LayoutMessage &lmsg)
{
  BOOL lborder = Const_Frame, tborder = Const_Frame;
  if(lmsg.LeftBorder != Const_ForceNoFrame)
    Flags |= FLG_Frame_BorderLeft;
  if(lmsg.TopBorder != Const_ForceNoFrame)
    Flags |= FLG_Frame_BorderTop;

  if(!FrameBorder)
  {
    lborder = Const_NoFrame;
    tborder = Const_NoFrame;

    if(lmsg.LeftBorder != Const_Frame)
      Flags &= ~FLG_Frame_BorderLeft;
    if(lmsg.TopBorder != Const_Frame)
      Flags &= ~FLG_Frame_BorderTop;
  }
  lmsg.LeftBorder = lborder;
  lmsg.TopBorder = tborder;

  if(HTMLview)
  {
    ULONG x = lmsg.MinX, y = lmsg.MinY;
    ULONG width = lmsg.MaxX - x + 1, height = lmsg.MaxY - y + 1;

    Left = x;
    Top = y;
    Right = x + width - 1;
    Bottom = y + height - 1;

    if(Flags & FLG_Frame_BorderLeft && width > 6)
    {
      x += 6;
      width -= 6;
    }
    else Flags &= ~FLG_Frame_BorderLeft;

    if(Flags & FLG_Frame_BorderTop && height > 6)
    {
      y += 6;
      height -= 6;
    }
    else Flags &= ~FLG_Frame_BorderTop;

    MUI_Layout(ScrollGroup, x, y, width, height, 0L);
    if(!(Flags & FLG_Frame_PageLoaded))
    {
      STRPTR url;
      if(Src && (url = (STRPTR)DoMethod(lmsg.HTMLview, MUIM_HTMLview_AddPart, Src)))
        DoMethod(HTMLview, MUIM_HTMLview_PrivateGotoURL, url, Name);
      Flags |= FLG_Frame_PageLoaded;
    }
    Flags |= FLG_Layouted;
  }

  return TRUE;
}

VOID FrameClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  AttrClass::Parse(pmsg);

  BOOL noresize = FALSE;
  const char *FrameKeywords[] = { "AUTO", "YES", "NO", NULL };
  struct ArgList args[] =
  {
    { "NAME",         &Name,          ARG_STRING,   NULL },
    { "SRC",          &Src,           ARG_URL,      NULL },
    { "NORESIZE",     &noresize,      ARG_SWITCH,   NULL },
    { "SCROLLING",    &Scrolling,     ARG_KEYWORD,  FrameKeywords },
    { "FRAMEBORDER",  &FrameBorder,   ARG_BOOLEAN,  NULL },
    { "MARGINWIDTH",  &MarginWidth,   ARG_NUMBER,   NULL },
    { "MARGINHEIGHT", &MarginHeight,  ARG_NUMBER,   NULL },
    { NULL,           NULL,           0,            NULL }
  };
  FrameBorder = pmsg.FrameBorder;
  MarginWidth = MarginHeight = 5;
  ScanArgs(pmsg.Locked, args);

  if(!Name)
  {
    Name = new char[10];
    sprintf(Name, "%08lx", (ULONG)this);
  }

  if(noresize)
    Flags |= FLG_Frame_NoResize;

  pmsg.Gadgets = TRUE;
}

VOID FrameClass::Render (struct RenderMessage &rmsg)
{
  struct RastPort *rp = rmsg.RPort;

  ULONG x1 = Left - rmsg.OffsetX, y1 = Top - rmsg.OffsetY,
      x2 = Right - rmsg.OffsetX, y2 = Bottom - rmsg.OffsetY;

  if(Flags & FLG_Frame_BorderTop)
  {
    SetAPen(rp, rmsg.Colours[Col_Background]);
    RectFill(rp, x1, y1, x2, y1+5);

    SetAPen(rp, rmsg.Colours[Col_Shine]);
    RectFill(rp, x1, y1+1, x2, y1+1);
    SetAPen(rp, rmsg.Colours[Col_Shadow]);
    RectFill(rp, x1, y1+4, x2, y1+4);

    y1 += 6;
  }

  if(Flags & FLG_Frame_BorderLeft)
  {
    SetAPen(rp, rmsg.Colours[Col_Background]);
    RectFill(rp, x1, y1, x1+5, y2);

    SetAPen(rp, rmsg.Colours[Col_Shine]);
    RectFill(rp, x1+1, y1, x1+1, y2);
    SetAPen(rp, rmsg.Colours[Col_Shadow]);
    RectFill(rp, x1+4, y1, x1+4, y2);
  }
}


