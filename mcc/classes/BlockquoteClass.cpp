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

#include "BlockquoteClass.h"

#include "Layout.h"
#include "MinMax.h"

BOOL BlockquoteClass::Layout (struct LayoutMessage &lmsg)
{
	if(Flags & FLG_Virgin)
	{
		lmsg.EnsureNewline();
		if(lmsg.MinY != lmsg.Y)
			lmsg.AddYSpace(4);
		lmsg.Indent += 30;
		lmsg.X += 30;
		lmsg.MaxX -= 30;
	}

	if(TreeClass::Layout(lmsg))
	{
		lmsg.AddYSpace(4);
		lmsg.Indent -= 30;
		lmsg.X -= 30;
		lmsg.MaxX += 30;
	}

  return TRUE;
}

VOID BlockquoteClass::MinMax (struct MinMaxMessage &mmsg)
{
	mmsg.Indent += 60;
	TreeClass::MinMax(mmsg);
	mmsg.X -= 60;
	mmsg.Indent -= 60;
}

