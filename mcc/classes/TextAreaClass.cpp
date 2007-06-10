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

#include "TextAreaClass.h"

#include "Entities.h"
#include "Forms.h"
#include "General.h"
#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

#include <proto/exec.h>
#include <mui/TextEditor_mcc.h>

VOID TextAreaClass::AppendGadget (struct AppendGadgetMessage &amsg)
{
  if(Flags & FLG_AllElementsPresent)
  {
    if(!MUIGadget)
    {
      MUIGadget = TextEditorObject,
        MUIA_TextEditor_Rows, Rows,
        MUIA_TextEditor_Columns, Columns,
/*        MUIA_FixWidth, 400,
        MUIA_FixHeight, 200,
*/        MUIA_TextEditor_InVirtualGroup, TRUE,
        MUIA_TextEditor_Contents, Contents,
        MUIA_TextEditor_FixedFont, TRUE,
        MUIA_TextEditor_ReadOnly, Flags & FLG_TextArea_ReadOnly,
        MUIA_CycleChain, TRUE,
        End;

      if(MUIGadget)
          DoMethod(amsg.Parent, OM_ADDMEMBER, MUIGadget);
      else  Flags |= FLG_Layouted;
    }
  }
}

VOID TextAreaClass::ExportForm (struct ExportFormMessage &emsg)
{
  if(MUIGadget)
  {
    STRPTR text;
    if((text = (STRPTR)DoMethod(MUIGadget, MUIM_TextEditor_ExportText)))
    {
      emsg.AddElement(Name, text);
      FreeVec(text);
    }
  }
}

VOID TextAreaClass::ResetForm ()
{

}

BOOL TextAreaClass::Layout (struct LayoutMessage &lmsg)
{
  if(~Flags & FLG_TextArea_ObjAdded)
  {
    lmsg.AddToGadgetList(this);
    Flags |= FLG_TextArea_ObjAdded;
  }

  if(Flags & FLG_AllElementsPresent)
  {
    if(MUIGadget)
    {
      Width = _minwidth(MUIGadget);
      Height = _minheight(MUIGadget);

      if(Width > (ULONG)lmsg.ScrWidth())
        lmsg.EnsureNewline();

      Left = lmsg.X;
      lmsg.X += Width;

      Top = lmsg.Y - (Height-1);
      Bottom = lmsg.Y;

      lmsg.UpdateBaseline(Height, Height-1);

      struct ObjectNotify *notify = new struct ObjectNotify(Left, Baseline, this);
      lmsg.AddNotify(notify);

      Flags |= FLG_WaitingForSize;
    }
  }

  return TRUE;
}
VOID TextAreaClass::AdjustPosition (LONG x, LONG y)
{
  Left += x;
  SuperClass::AdjustPosition(x, y);
  if(MUIGadget && Width && Height)
    MUI_Layout(MUIGadget, Left, Top, Width, Height, 0L);
}

VOID TextAreaClass::MinMax (struct MinMaxMessage &mmsg)
{
  if(MUIGadget)
  {
    ULONG width = _minwidth(MUIGadget);
    mmsg.X += width;
    mmsg.Min = max(width, (ULONG)mmsg.Min);
  }
}

VOID TextAreaClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  BOOL disabled = FALSE, readonly = FALSE;
  struct ArgList args[] =
  {
    { "NAME",     &Name,      ARG_STRING, NULL  },
    { "DISABLED", &disabled,  ARG_SWITCH, NULL  },
    { "READONLY", &readonly,  ARG_SWITCH, NULL  },
    { "COLS",     &Columns,   ARG_NUMBER, NULL  },
    { "ROWS",     &Rows,      ARG_NUMBER, NULL  },
    { NULL,       NULL,       0,          NULL }
  };

  Rows = 10; Columns = 40;
  ScanArgs(pmsg.Locked, args);

  if(disabled)
    Flags |= FLG_TextArea_Disabled;
  if(readonly)
    Flags |= FLG_TextArea_ReadOnly;

  pmsg.SetLock();
  pmsg.NextStartBracket();

  Contents = new char[pmsg.Current-pmsg.Locked+1];
  STRPTR src = pmsg.Locked, dst = Contents;
  while(*src != '<' && *src)
  {
    struct EntityInfo *entity;
    if(*src == '&' && (entity = GetEntityInfo(src+1)))
    {
      *dst++ = entity->ByteCode;
      src += strlen(entity->Name)+1;
      if(*src == ';')
        src++;
    }
    else
    {
      *dst++ = *src++;
    }
  }
  *dst = '\0';

  Flags |= FLG_AllElementsPresent | FLG_ArgumentsRead;;
  pmsg.Gadgets = TRUE;
}

VOID TextAreaClass::Render (struct RenderMessage &rmsg)
{
  if(rmsg.RedrawGadgets && MUIGadget)
    MUI_Redraw(MUIGadget, MADF_DRAWOBJECT | MADF_DRAWOUTER);
}


