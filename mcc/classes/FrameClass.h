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

#ifndef FRAMECLASS_H
#define FRAMECLASS_H

#include "AttrClass.h"

// forward declarations

class FrameClass : public AttrClass
{
  public:
    FrameClass () : AttrClass() { ; }
    ~FrameClass () { delete Name; delete Src; }
    VOID Parse (struct ParseMessage &pmsg);
    VOID AppendGadget (struct AppendGadgetMessage &amsg);
    BOOL Layout (struct LayoutMessage &lmsg);
    VOID Render (struct RenderMessage &rmsg);
    Object *LookupFrame (STRPTR name, class HostClass *hclass);
    Object *HandleMUIEvent (struct MUIP_HandleEvent *hmsg);
    BOOL HitTest (struct HitTestMessage &hmsg);
    Object *FindDefaultFrame (ULONG &size);

  protected:
    STRPTR Name, Src;
    LONG Left, Right;
    ULONG Scrolling, FrameBorder;
    ULONG MarginWidth, MarginHeight;
    Object *ScrollGroup, *HTMLview;
};

#endif // FRAMECLASS_H
