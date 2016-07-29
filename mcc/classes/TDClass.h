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

#ifndef TDCLASS_H
#define TDCLASS_H

#include "BackFillClass.h"

// forward declarations

class TDClass : public BackFillClass
{
  public:
    TDClass () : BackFillClass()
    {
      Group = group_Tablecell;
      Flags |= FLG_Newline;
      ColSpan = RowSpan = 1;
      VAlignment = Align_Middle;
    }
    ~TDClass () { delete GivenWidth; }
    virtual VOID Parse (struct ParseMessage &pmsg);
    VOID TDMinMax (struct MinMaxMessage &mmsg);
    BOOL TDLayout (struct LayoutMessage &lmsg);
    VOID AdjustPosition (LONG x, LONG y);
    virtual VOID TDRender (struct RenderMessage &rmsg);
    BOOL Mark (struct MarkMessage &mmsg);

    ULONG Width, Height, Min, Max;
    ULONG ColSpan, RowSpan;
    LONG Left, Alignment, VAlignment;
    struct ArgSize *GivenWidth;
};

#endif // TDCLASS_H
