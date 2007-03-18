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

#ifndef TRCLASS_H
#define TRCLASS_H

#include "BackFillClass.h"

// forward declarations

class TRClass : public BackFillClass
{
	public:
		TRClass () : BackFillClass() { Flags |= FLG_Newline; }
		~TRClass () { FirstChild = RealFirstChild; }
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		VOID TRMinMax (struct MinMaxMessage &mmsg);
		BOOL TRLayout (struct LayoutMessage &lmsg);
		VOID CountCells (struct CountCellsMessage &cmsg);
		VOID TRRender (struct RenderMessage &rmsg);
		VOID Verify ();
		BOOL Mark (struct MarkMessage &mmsg);

		struct ChildsList *RealFirstChild;
};

#endif // TRCLASS_H
