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

#include "BodyClass.h"

#include "General.h"
#include "Layout.h"
#include "ParseMessage.h"
#include "ScanArgs.h"
#include "SharedData.h"

BodyClass::~BodyClass ()
{
  FreeColours(CMap);
}

VOID BodyClass::AllocateColours (struct ColorMap *cmap)
{
  CMap = cmap;
  BackgroundCol = AllocPen(cmap, BackgroundRGB);
  TextCol     = AllocPen(cmap, TextRGB);
  LinkCol     = AllocPen(cmap, LinkRGB);
  VLinkCol      = AllocPen(cmap, VLinkRGB);
  ALinkCol      = AllocPen(cmap, ALinkRGB);

#ifdef TRACE_ALLOCATIONS
  AllocatedColours += 2;
#endif
  if(BackgroundCol != -1)
      ObtainShineShadowPens(cmap, BackgroundRGB, HalfshineCol, HalfshadowCol);
  else  HalfshadowCol = HalfshineCol = -1;

  if(!(Flags & FLG_AllocatedColours))
  {
    Flags |= FLG_AllocatedColours;
    BackFillClass::AllocateColours(cmap);
  }
}

VOID BodyClass::FreeColours (struct ColorMap *cmap UNUSED)
{
  if(Flags & FLG_AllocatedColours)
  {
    FreePen(CMap, BackgroundCol);
    FreePen(CMap, TextCol);
    FreePen(CMap, LinkCol);
    FreePen(CMap, VLinkCol);
    FreePen(CMap, ALinkCol);
    FreePen(CMap, HalfshineCol);
    FreePen(CMap, HalfshadowCol);

    Flags &= ~FLG_AllocatedColours;
  }
  BackFillClass::FreeColours(CMap);
}

BOOL BodyClass::HitTest (struct HitTestMessage &hmsg)
{
  hmsg.Background = Source;
  return(TreeClass::HitTest(hmsg));
}

BOOL BodyClass::Layout (struct LayoutMessage &lmsg)
{
  if(!(Flags & FLG_ArgumentsRead) || Flags & FLG_Layouted)
    return(FALSE);

  if(Flags & FLG_Virgin)
  {
    lmsg.MinX += lmsg.MarginWidth;
    lmsg.MinY += lmsg.MarginHeight;
    lmsg.MaxX -= lmsg.MarginWidth;
    lmsg.MaxY -= lmsg.MarginHeight;
    lmsg.X = lmsg.MarginWidth;
    lmsg.Y = lmsg.MarginHeight;

    lmsg.ScrHeight = lmsg.MaxY - lmsg.MinY + 1;

    Top = lmsg.TopChange = 0;

    FindImage(lmsg);

    if(!(Flags & FLG_AllocatedColours))
    {
      Flags |= FLG_AllocatedColours;
      AllocateColours(lmsg.Share->Scr->ViewPort.ColorMap);
    }
  }

  lmsg.LIIndent = lmsg.Share->LI_Width;
  if(TreeClass::Layout(lmsg))
  {
    Top = 0;
    lmsg.EnsureNewline();
    lmsg.FlushImages(Flush_All);
    lmsg.AddYSpace(5);
    Bottom = lmsg.Height = max(lmsg.MaxY-1+lmsg.MarginHeight, lmsg.Y);

    //D(DBF_ALWAYS, "LayoutStack: %ld", LayoutStack);
  }

  return TRUE;
}

VOID BodyClass::Parse(struct ParseMessage &pmsg)
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  struct ArgList args[] =
  {
    { "BACKGROUND", &Source,        ARG_URL,    NULL },
    { "BGCOLOR",    &BackgroundRGB, ARG_COLOUR, NULL },

    { "TEXT",       &TextRGB,       ARG_COLOUR, NULL },
    { "LINK",       &LinkRGB,       ARG_COLOUR, NULL },
    { "VLINK",      &VLinkRGB,      ARG_COLOUR, NULL },
    { "ALINK",      &ALinkRGB,      ARG_COLOUR, NULL },
    { NULL,         NULL,           0,          NULL }
  };
  ScanArgs(pmsg.Locked, args);

  pmsg.PendingNewline = TRUE;
  BackFillClass::Parse(pmsg);
}

VOID BodyClass::Render (struct RenderMessage &rmsg)
{
  if(Flags & FLG_ArgumentsRead)
  {
    if(!(rmsg.Share->Flags & FLG_CustomColours))
    {
      SetCol(rmsg.Colours[Col_Background], BackgroundCol);
      SetCol(rmsg.Colours[Col_Text], TextCol);
      SetCol(rmsg.Colours[Col_Link], LinkCol);
      SetCol(rmsg.Colours[Col_VLink], VLinkCol);
      SetCol(rmsg.Colours[Col_ALink], ALinkCol);
      SetCol(rmsg.Colours[Col_Halfshine], HalfshineCol);
      SetCol(rmsg.Colours[Col_Halfshadow], HalfshadowCol);
    }

    if(rmsg.TargetObj == (class SuperClass *)this)
      rmsg.TargetObj = NULL;

    if(!rmsg.TargetObj)
      DrawBackground(rmsg, rmsg.MinX, rmsg.MinY, rmsg.MaxX, rmsg.MaxY, rmsg.Left, rmsg.Top);

    rmsg.BackgroundObj = this;
    TreeClass::Render(rmsg);

    /* For other MUI objects */
    SetSoftStyle(rmsg.RPort, 0L, TSF_StyleMask);
  }
}

