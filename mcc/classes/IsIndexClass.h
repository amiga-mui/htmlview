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

#ifndef ISINDEXCLASS_H
#define ISINDEXCLASS_H

#include "AttrClass.h"

// forward declarations

class IsIndexClass : public AttrClass
{
	public:
		IsIndexClass () : AttrClass() { Flags |= FLG_Gadget; }
		~IsIndexClass () { delete Prompt; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID MinMax (struct MinMaxMessage &mmsg);
		VOID AppendGadget (struct AppendGadgetMessage &amsg);
		BOOL Layout (struct LayoutMessage &lmsg);
		VOID AdjustPosition (LONG x, LONG y);
		VOID Render (struct RenderMessage &rmsg);

	protected:
		LONG Left, Baseline;
		STRPTR Prompt;
		Object *MUIGadget;
		ULONG Width, Height;
};

#endif // ISINDEXCLASS_H
