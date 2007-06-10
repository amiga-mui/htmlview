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

#ifndef BACKFILLCLASS_H
#define BACKFILLCLASS_H

#include "TreeClass.h"

// forward declarations

class BackFillClass : public TreeClass
{
  public:
    BackFillClass () : TreeClass() { BackgroundRGB = -1; }
    ~BackFillClass ();
    VOID Parse (REG(a2, struct ParseMessage &pmsg));

    VOID FindImage (struct LayoutMessage &lmsg);
    VOID GetImages (struct GetImagesMessage &gmsg);
    BOOL ReceiveImage (struct PictureFrame *pic);
    BOOL UpdateImage (LONG ystart, LONG ystop, LONG top, LONG bottom, BOOL last);
    BOOL FlushImage (LONG top, LONG bottom);
    BOOL ReadyForAlpha ();
    BOOL Complex ();

    VOID AllocateColours (struct ColorMap *cmap);
    VOID FreeColours (struct ColorMap *cmap);

    VOID DrawBackground (struct RenderMessage &rmsg, LONG left, LONG top, LONG width, LONG height, LONG xoffset, LONG yoffset);

  protected:
    STRPTR Source;
    struct PictureFrame *Picture;
    LONG BackgroundRGB, BackgroundCol;
};

#endif // BACKFILLCLASS_H
