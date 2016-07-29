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

#ifndef TEXTCLASS_H
#define TEXTCLASS_H

#include "SuperClass.h"

// forward declarations

class TextClass : public SuperClass
{
  public:
    TextClass () : SuperClass() { ID = tag_Text; LineInfoLast = (struct TextLineInfo *)&LineInfo; }
    ~TextClass ()
    {
      delete Contents;
      DeleteLineInfo();
    }

    VOID Parse (struct ParseMessage &pmsg);
    VOID MinMax (struct MinMaxMessage &mmsg);
    BOOL Layout (struct LayoutMessage &lmsg);
    VOID Relayout (BOOL all);
    VOID AdjustPosition (LONG x, LONG y);
    VOID Render (struct RenderMessage &rmsg);
    BOOL HitTest (struct HitTestMessage &hmsg);
    VOID DeleteLineInfo ();
    class TextClass *Find (struct FindMessage &fmsg);
    BOOL Mark (struct MarkMessage &mmsg);
    LONG FindChar (LONG x, LONG y, BOOL newline);
    VOID RenderMarked (struct RastPort *rp, LONG markbegin, LONG markend, LONG xoffset, LONG yoffset);

    LONG MarkBegin, MarkEnd;
    STRPTR Contents;

  protected:
    struct TextLineInfo *LineInfo;
    struct TextLineInfo *LineInfoLast;
    LONG Left;
    ULONG Length;
    struct TextFont *Font;
};

#endif // TEXTCLASS_H
