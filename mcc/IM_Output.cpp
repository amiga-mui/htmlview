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

#include <exec/types.h>

#include "SDI_compiler.h"

#include "IM_Output.h"
#include "General.h"

#warning "FAST_DITHER is off, check!"
//#define FAST_DITHER

VOID TrueColourEngine::RenderLine (struct RGBPixel *dstline, ULONG y)
{
	RenderObj.WriteLine(DstWidth, dstline, y);
}

/* ---- */
/*
VOID HighColourEngine::RenderLine (struct RGBPixel *dstline, ULONG y)
{
	if(y-- && DstLastLine)
	{
		if(y & 1)
		{
			struct RGBPixel *line = DstLastLine + DstWidth-1;
			for(WORD x = DstWidth-1; x >= 0; x--)
			{
				UBYTE r = (*line).R, tr = r&0xf8;
				UBYTE g = (*line).G, tg = g&0xfc;
				UBYTE b = (*line).B, tb = b&0xf8;

				SpreadRToL(line, dstline+x, r - tr, g - tg, b - tb);
				line--;
			}
		}
		else
		{
			struct RGBPixel *line = DstLastLine;
			for(UWORD x = 0; x < DstWidth; x++)
			{
				UBYTE r = (*line).R, tr = r&0xf8;
				UBYTE g = (*line).G, tg = g&0xfc;
				UBYTE b = (*line).B, tb = b&0xf8;

				SpreadLToR(line, dstline+x, r - tr, g - tg, b - tb);
				line++;
			}
		}
		RenderObj.WriteLine(DstWidth, DstLastLine, y);
	}
	DstLastLine = dstline;
}
*/
/* ---- */

LowColourNDEngine::~LowColourNDEngine ()
{
//	ReleaseCMap(CMap);
	delete ChunkyLine;
}

BOOL LowColourNDEngine::SetDimensions (ULONG width, ULONG height, BOOL interlaced, ULONG animdelay, ULONG disposal, ULONG leftofs, ULONG topofs, struct RGBPixel *background, ULONG transparency)
{
	BOOL result = ScaleEngine::SetDimensions(width, height, interlaced, animdelay, disposal, leftofs, topofs, background, transparency);

	if(result && !CMap)
	{
		RenderObj.SetCMap(CMap = ObtainCMap(Scr));
		ChunkyLine = new UBYTE [DstWidth];

		if(!(CMap && ChunkyLine))
			result = FALSE;
	}
	return(result);
}

VOID LowColourNDEngine::RenderLine (struct RGBPixel *dstline, ULONG y)
{
	UBYTE *chunky = ChunkyLine;

	struct ColourManager *cmap = CMap;
	for(ULONG x = 0; x < DstWidth; x++)
		*chunky++ = cmap->Allocated[(*dstline++).Index()];

	RenderObj.WriteLine(DstWidth, ChunkyLine, y);
}

/* ---- */
/*
inline VOID Round (UBYTE *col, WORD *add)
{
	if(*add > 0)
			if(*col + *add > 255)	*col = 255	else	*col += *add;
	else	if(*col + *add < 0)		*col = 0		else	*col += *add;
}
*/

extern "C" VOID FSDither (
	REG(a0, struct RGBPixel *line),
	REG(a1, UBYTE *cunky),
	REG(a2, LONG *allocated),
	REG(a3, struct RGBPixel *cmap),
	REG(a4, struct RGBPixel *below),
	REG(d0, ULONG width));

VOID LowColourEngine::RenderLine (struct RGBPixel *dstline, ULONG y)
{
	if(y-- && DstLastLine)
	{
#ifdef FAST_DITHER
		FSDither(DstLastLine, ChunkyLine, CMap->Allocated, CMap->CValues, dstline-1, DstWidth);
#else
		UBYTE *chunky = ChunkyLine;
		if(y & 1)
		{
			struct RGBPixel *line = DstLastLine + DstWidth-1;
			for(WORD x = DstWidth-1; x >= 0; x--)
			{
				ULONG index = (*line).Index();
				chunky[x] = CMap->Allocated[index];

				struct RGBPixel got = CMap->CValues[index], want = *line--;
				SpreadRToL(line, dstline+x, want.R - got.R, want.G - got.G, want.B - got.B);
			}
		}
		else
		{
			struct RGBPixel *line = DstLastLine;
			for(UWORD x = 0; x < DstWidth; x++)
			{
				ULONG index = (*line).Index();
				*chunky++ = CMap->Allocated[index];

				struct RGBPixel got = CMap->CValues[index], want = *line++;
				SpreadLToR(line, dstline+x, want.R - got.R, want.G - got.G, want.B - got.B);
			}
		}
#endif
		RenderObj.WriteLine(DstWidth, ChunkyLine, y);
	}
	DstLastLine = dstline;
}
