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

#ifndef IMGCLASS_H
#define IMGCLASS_H

#include "AttrClass.h"

// forward declarations

class ImgClass : public AttrClass
{
  public:
    ImgClass () : AttrClass() { ; }
    ~ImgClass ();
    VOID Parse (struct ParseMessage &pmsg);
    VOID MinMax (struct MinMaxMessage &mmsg);
    BOOL Layout (struct LayoutMessage &lmsg);
    VOID Relayout (BOOL all);
    VOID AdjustPosition (LONG x, LONG y);
    VOID GetImages (struct GetImagesMessage &gmsg);
    VOID Render (struct RenderMessage &rmsg);
    VOID FreeColours (struct ColorMap *cmap);
    BOOL HitTest (struct HitTestMessage &hmsg);

    BOOL ReceiveImage (struct PictureFrame *pic);
    BOOL UpdateImage (LONG ystart, LONG ystop, LONG top, LONG bottom, BOOL last);
    BOOL FlushImage (LONG top, LONG bottom);

    LONG Width (LONG width = 80, struct LayoutMessage *lmsg = NULL);
    LONG Height (LONG height = 20, struct LayoutMessage *lmsg = NULL);

    LONG YStart, YStop;
    struct PictureFrame *Picture;
    STRPTR AltText;
    ULONG Alignment;

  protected:
    LONG Left, Baseline;
    struct ArgSize *GivenWidth, *GivenHeight;
    struct BitMap *BlendBitMap;
    STRPTR Source, Name;
    ULONG ImgBorder;
    STRPTR Map;
    class MapClass *MapObj;
    ULONG HSpace, VSpace;

    BOOL TestPixel(UBYTE *line, LONG x);
};

#endif // IMGCLASS_H
