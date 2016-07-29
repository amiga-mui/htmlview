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

#ifndef TABLECLASS_H
#define TABLECLASS_H

#include "BackFillClass.h"
#include "MinMax.h"

// forward declarations

class TableClass : public BackFillClass
{
  public:
    TableClass()
      : BackFillClass()
    {
      Flags |= FLG_Newline;
    }

    ~TableClass()
    {
      delete Widths;
      delete Heights;
      delete GivenWidth;
      delete RowOpenCounts;
      FirstChild = RealFirstChild;
    }

    VOID Parse (struct ParseMessage &pmsg);
    VOID MinMax (struct MinMaxMessage &mmsg);
    BOOL Layout (struct LayoutMessage &lmsg);
    VOID Relayout (BOOL all);
    VOID AdjustPosition (LONG x, LONG y);
    VOID Render (struct RenderMessage &rmsg);
    class SuperClass *FindElement (ULONG tagID);
    BOOL Mark (struct MarkMessage &mmsg);
    VOID Spread (LONG scale, LONG width, LONG (*get_width) (struct CellWidth *));

  protected:
    VOID DrawCorner(struct RastPort *rport, LONG x, LONG y, UWORD width, UBYTE col1, UBYTE col2);

  protected:
    ULONG Rows, Columns, Min, Max, Width, Height;
    ULONG *Heights, *RowOpenCounts;
    struct CellWidth *Widths;
    LONG Left, Spacing, Padding, BorderSize, Alignment;
    struct ArgSize *GivenWidth;
    struct ChildsList *RealFirstChild;
};

#endif // TABLECLASS_H
