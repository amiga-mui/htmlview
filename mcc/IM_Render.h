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

#ifndef SCALEENGINE_H
#include "IM_Scale.h"
#endif

#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#ifndef GRAPHICS_RASTPORT_H
#include <graphics/rastport.h>
#endif

#include "IM_ColourManager.h"

struct PictureFrame
{
	PictureFrame (ULONG width, ULONG height, ULONG leftofs, ULONG topofs, ULONG animdelay, ULONG disposal, struct RGBPixel *background, struct BitMap *bmp, UBYTE *mask, ULONG flags);
	~PictureFrame ();
	VOID LockPicture ();
	VOID UnLockPicture ();
	ULONG Size ();
	BOOL MatchSize (ULONG width, ULONG height);

	struct PictureFrame *Next;
	LONG Width, Height, Depth; /* Depth is only > 1 when interleaved */
	ULONG LeftOfs, TopOfs;
	ULONG AnimDelay, LoopCnt, Disposal;
	ULONG ScrWidth, ScrHeight;
	struct RGBPixel Background;
	struct BitMap *BMp;
	UBYTE *Mask, *AlphaMask;
	class ColourManager *CMap;
	ULONG LockCnt;
	ULONG Flags;
};

#define PicFLG_Full     (1 << 0)
#define PicFLG_ScaledX  (1 << 1)
#define PicFLG_ScaledY  (1 << 2)
#define PicFLG_Complete (1 << 3)

class RenderEngine
{
	public:
		RenderEngine (struct Screen *scr, struct DecoderData *data);
		~RenderEngine ();

		VOID SetCMap (class ColourManager *cmap);
		VOID WriteLine (ULONG width, struct RGBPixel *line, ULONG y);
		VOID WriteLine (ULONG width, UBYTE *chunky, ULONG y);
		BOOL AllocateFrame (ULONG width, ULONG height, ULONG animdelay, ULONG disposal, ULONG leftofs, ULONG topofs, struct RGBPixel *background, ULONG transparency, ULONG flags);
		VOID WriteMask (struct RGBPixel *srcline, ULONG width, ULONG y);
		VOID Multiply (ULONG width, ULONG y, ULONG height);

	protected:
		struct PictureFrame *FirstFrame;
		struct PictureFrame *LastFrame;
		struct Screen *Scr;
		struct DecoderData *Data;
		struct RastPort TmpRP, BMpRP;
		UBYTE *Mask, *AlphaMask;
};

#endif
