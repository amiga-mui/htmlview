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

#include "SelectClass.h"
#include "OptionClass.h"

#include "Forms.h"
#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "private.h"
#include "ScanArgs.h"

VOID SelectClass::AppendGadget (struct AppendGadgetMessage &amsg)
{
  if(Flags & FLG_AllElementsPresent)
  {
    if(!MUIGadget)
    {
      MUIGadget = CycleObject,
        MUIA_Cycle_Active, Active,
        MUIA_Cycle_Entries, Entries,
        MUIA_CycleChain, TRUE,
        End;

      if(MUIGadget)
          DoMethod(amsg.Parent, OM_ADDMEMBER, MUIGadget);
      else  Flags |= FLG_Layouted;
    }
  }
}

VOID SelectClass::ExportForm (struct ExportFormMessage &emsg)
{
  if(MUIGadget)
  {
    ULONG active = xget(MUIGadget, MUIA_Cycle_Active);
    emsg.AddElement(Name, Values[active]);
  }
}

VOID SelectClass::ResetForm ()
{

}

BOOL SelectClass::Layout (struct LayoutMessage &lmsg)
{
  if(!(Flags & FLG_Select_ObjAdded))
  {
    lmsg.AddToGadgetList(this);
    Flags |= FLG_Select_ObjAdded;
  }

  if(Flags & FLG_AllElementsPresent)
  {
    if(MUIGadget)
    {
      Width = _minwidth(MUIGadget) + 4;
      Height = _minheight(MUIGadget) + 2;

      if(Width > (ULONG)lmsg.ScrWidth())
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
  }

   return TRUE;
}

VOID SelectClass::AdjustPosition (LONG x, LONG y)
{
  Left += x;
  SuperClass::AdjustPosition(x, y);
  if(MUIGadget && Width && Height)
    MUI_Layout(MUIGadget, Left, Top, Width-4, Height-2, 0L);
}

VOID SelectClass::MinMax (struct MinMaxMessage &mmsg)
{
  if(MUIGadget)
  {
    ULONG width = _minwidth(MUIGadget);
    mmsg.X += width;
    mmsg.Min = max(width, (ULONG)mmsg.Min);
  }
}

VOID SelectClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  BOOL disabled = FALSE, multiple = FALSE;
  struct ArgList args[] =
  {
    { "NAME",   &Name,    ARG_STRING, NULL  },
    { "SIZE",   &Size,    ARG_NUMBER, NULL  },
    { "DISABLED", &disabled,  ARG_SWITCH, NULL  },
    { "MULTIPLE", &multiple,  ARG_SWITCH, NULL  },
    { NULL,        NULL,       0,          NULL  }
  };

  ScanArgs(pmsg.Locked, args);
  if(disabled)
    Flags |= FLG_Select_Disabled;
  if(multiple)
    Flags |= FLG_Select_Multiple;

  TreeClass::Parse(pmsg);

  ULONG entries = 0;
  struct ChildsList *first = FirstChild;
  while(first)
  {
    if(first->Obj->id() == tag_OPTION && !(first->Obj->flags() & FLG_Option_Disabled))
      entries++;

    first = first->Next;
  }

  Values = new STRPTR [entries+1];
  Entries = new STRPTR [entries+1];
  entries = 0;
  first = FirstChild;
  Active = 0;
  while(first)
  {
    if(first->Obj->id() == tag_OPTION && !(first->Obj->flags() & FLG_Option_Disabled))
    {
      class OptionClass *option = ((class OptionClass *)first->Obj);
      STRPTR contents = option->Contents, value = option->Value;
      Entries[entries] = contents ? contents : (STRPTR)"<unnamed>";
      Values[entries] = value ? value : contents;
      if(option->flags() & FLG_Option_Selected)
        Active = entries;
      entries++;
    }
    first = first->Next;
  }
  Entries[entries] = NULL;

  Flags |= FLG_AllElementsPresent;
  pmsg.Gadgets = TRUE;
}

VOID SelectClass::Render (struct RenderMessage &rmsg)
{
  if(MUIGadget && rmsg.RedrawGadgets)
  {
    DoMethod(MUIGadget, MUIM_DrawBackground, Left-rmsg.OffsetX, Top-rmsg.OffsetY, Width-4, Height-2, 0, 0, 0L);
    MUI_Redraw(MUIGadget, MADF_DRAWOBJECT | MADF_DRAWOUTER);
  }
}

