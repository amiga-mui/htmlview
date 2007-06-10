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

#include "IsIndexClass.h"

#include "General.h"
#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

#include <mui/BetterString_mcc.h>

VOID IsIndexClass::AppendGadget (struct AppendGadgetMessage &amsg)
{
  if(!MUIGadget)
  {
    MUIGadget = BetterStringObject,
                  StringFrame,
                  MUIA_BetterString_Columns, 40,
                  End;

    if(MUIGadget)
    {
      DoMethod(MUIGadget, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, amsg.Parent, 2, MUIM_HTMLview_ServerRequest, MUIV_TriggerValue);
      DoMethod(amsg.Parent, OM_ADDMEMBER, MUIGadget);
    }
    else
      Flags |= FLG_Layouted;
  }
}

BOOL IsIndexClass::Layout (struct LayoutMessage &lmsg)
{
  if(!(Flags & FLG_IsIndex_ObjAdded))
  {
    lmsg.AddToGadgetList(this);
    Flags |= FLG_IsIndex_ObjAdded;
  }

  if(MUIGadget)
  {
    Width = _minwidth(MUIGadget) + 4;
    Height = _minheight(MUIGadget) + 2;

    if((LONG)Width > lmsg.ScrWidth())
      lmsg.EnsureNewline();

    Left = lmsg.X+2;
    lmsg.X += Width;

    Top = lmsg.Y - (Height-7);
    Bottom = lmsg.Y;

    lmsg.UpdateBaseline(Height, Height-7);

    struct ObjectNotify *notify = new struct ObjectNotify(Left, Baseline, this);
    lmsg.AddNotify(notify);

    Flags |= FLG_WaitingForSize;
  }
  else
  {
    Flags |= FLG_Layouted;
  }

   return TRUE;
}

VOID IsIndexClass::AdjustPosition (LONG x, LONG y)
{
  Left += x;
  SuperClass::AdjustPosition(x, y);
  if(MUIGadget && Width && Height)
    MUI_Layout(MUIGadget, Left, Top, Width-4, Height-2, 0L);
}

VOID IsIndexClass::MinMax (struct MinMaxMessage &mmsg)
{
  if(MUIGadget)
  {
    ULONG width = _minwidth(MUIGadget);
    mmsg.X += width;
    mmsg.Min = max((LONG)width, mmsg.Min);
  }
}

VOID IsIndexClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  AttrClass::Parse(pmsg);

  struct ArgList args[] =
  {
    { "PROMPT",   &Prompt,    ARG_STRING, NULL  },
    { NULL,        NULL,       0,          NULL  }
  };

  ScanArgs(pmsg.Locked, args);

  pmsg.Gadgets = TRUE;
}

VOID IsIndexClass::Render (struct RenderMessage &rmsg)
{
  if(rmsg.RedrawGadgets && MUIGadget)
    MUI_Redraw(MUIGadget, MADF_DRAWOBJECT | MADF_DRAWOUTER);
}

