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

#include "ULClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "SharedData.h"

BOOL ULClass::Layout (struct LayoutMessage &lmsg)
{
	if(Flags & FLG_Virgin)
	{
		lmsg.AddYSpace(4);
		lmsg.Indent += 20;
		lmsg.X += 20;
	}

	ULONG old_indent = lmsg.LIIndent;
	lmsg.LIIndent = lmsg.Share->LI_Width;

	if(TreeClass::Layout(lmsg))
	{
		lmsg.AddYSpace(4);
		lmsg.Indent -= 20;
		lmsg.X -= 20;
	}

	lmsg.LIIndent = old_indent;

  return TRUE;
}

VOID ULClass::MinMax (struct MinMaxMessage &mmsg)
{
	mmsg.Indent += 20;
	TreeClass::MinMax(mmsg);
	mmsg.X -= 20;
	mmsg.Indent -= 20;
}

VOID ULClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	APTR handle;
	if((handle = Backup(pmsg, 2, tag_LI, tag_OL)))
	{
		TreeClass::Parse(pmsg);
		Restore(pmsg.OpenCounts, 2, handle);
	}
}

VOID ULClass::Render (struct RenderMessage &rmsg)
{
	rmsg.UL_Nesting++;
	TreeClass::Render(rmsg);
	rmsg.UL_Nesting--;
}

