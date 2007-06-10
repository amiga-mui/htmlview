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

#ifndef SCALE_H
#define SCALE_H

#ifndef INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif

#include "IM_Render.h"

class ScaleEngine
{
  public:
    ScaleEngine (struct Screen *scr, ULONG width, ULONG height, struct DecoderData *data);
    virtual ~ScaleEngine ();
    virtual BOOL SetDimensions (ULONG width, ULONG height, BOOL interlaced, ULONG animdelay, ULONG disposal, ULONG leftofs, ULONG topofs, struct RGBPixel *background, ULONG transparency);

    virtual struct RGBPixel *GetLineBuffer (ULONG y, BOOL lastpass);
    virtual VOID DrawLineBuffer (struct RGBPixel *srcline, ULONG y, ULONG height);
    virtual VOID RenderLine (struct RGBPixel *dstline, ULONG y) = 0;
    virtual VOID RenderLineND (struct RGBPixel *dstline, ULONG y) = 0;
    virtual VOID FlushBuffers ();

    class RenderEngine RenderObj;

  protected:
    struct RGBPixel *ScaleX (struct RGBPixel *srcline, ULONG y);

    ULONG OrgWidth, OrgHeight;
    ULONG ScaleWidth, ScaleHeight;
    ULONG DstWidth, DstHeight;
    ULONG SrcWidth, SrcHeight;
    struct RGBPixel *SrcBuffer, *SrcLineA, *SrcLineB, *SrcLastLine;
    struct RGBPixel *ScaleBuffer, *XScaledLineA, *XScaledLineB;
    struct RGBPixel *DstBuffer, *DstLineA, *DstLineB, *DstLastLine;
    struct Screen *Scr;
    BOOL Interlaced, Dither, NotFirstFrame, LastPass;
};

#endif
