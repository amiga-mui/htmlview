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

#include "DLClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"

BOOL DLClass::Layout (struct LayoutMessage &lmsg)
{
  if(Flags & FLG_Virgin)
  {
    lmsg.AddYSpace(4);
    lmsg.X += 5;
    lmsg.Indent += 5;
  }

  if(TreeClass::Layout(lmsg))
  {
    lmsg.AddYSpace(4);
    lmsg.X -= 5;
    lmsg.Indent -= 5;
  }

  return TRUE;
}

VOID DLClass::MinMax (struct MinMaxMessage &mmsg)
{
  mmsg.Indent += 5;
  TreeClass::MinMax(mmsg);
  mmsg.X -= 5;
  mmsg.Indent -= 5;
}

VOID DLClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  UBYTE group = pmsg.OpenGroups[group_Definition];
  pmsg.OpenGroups[group_Definition] = 0;
  TreeClass::Parse(pmsg);
  pmsg.OpenGroups[group_Definition] = group;
}

