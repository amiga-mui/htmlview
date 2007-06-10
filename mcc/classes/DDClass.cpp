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

#include "DDClass.h"

#include "Layout.h"
#include "MinMax.h"

BOOL DDClass::Layout (struct LayoutMessage &lmsg)
{
  if(Flags & FLG_Virgin)
  {
    lmsg.EnsureNewline();
    lmsg.X += 15;
    lmsg.Indent += 15;

    lmsg.MinY = lmsg.Y; // This indicates that no additional linebreaks should be placed infront of P-tags etc.
  }

  if(TreeClass::Layout(lmsg))
  {
    lmsg.EnsureNewline();
    lmsg.X -= 15;
    lmsg.Indent -= 15;
  }

  return TRUE;
}

VOID DDClass::MinMax (struct MinMaxMessage &mmsg)
{
  mmsg.Indent += 15;
  TreeClass::MinMax(mmsg);
  mmsg.X -= 15;
  mmsg.Indent -= 15;
}

