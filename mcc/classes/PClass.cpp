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

#include "PClass.h"

#include "Layout.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

BOOL PClass::Layout (struct LayoutMessage &lmsg)
{
  if(Flags & FLG_Virgin)
  {
    lmsg.EnsureNewline();
    if(lmsg.MinY != lmsg.Y)
    {
      lmsg.UpdateBaseline(lmsg.Font->tf_YSize+1, lmsg.Font->tf_Baseline);
      lmsg.Newline();
    }
  }

  UBYTE oldalign = lmsg.Align;
  if(Alignment)
    lmsg.Align = Alignment;

  if(TreeClass::Layout(lmsg))
    lmsg.EnsureNewline();

  lmsg.Align = oldalign;

   return TRUE;
}

BOOL PClass::Mark (struct MarkMessage &mmsg)
{
  if(mmsg.Enabled())
  {
    if(!mmsg.Newline)
      mmsg.WriteLF();
    mmsg.WriteLF();
    mmsg.Newline = TRUE;
  }
  return(TreeClass::Mark(mmsg));
}

VOID PClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  struct ArgList args[] =
  {
    { "ALIGN",  &Alignment,   ARG_KEYWORD, AlignKeywords },
    { NULL,     NULL,          0,           NULL }
  };

  Alignment = (ULONG)-1;
  ScanArgs(pmsg.Locked, args);
  Alignment++;

  TreeClass::Parse(pmsg);
}

