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

#include <stdio.h>
#include <exec/types.h>

#include "IM_ColourManager.h"
#include "IM_Dither.h"

//#define FAST

VOID DitherEngine::FlushBuffers ()
{
	ScaleEngine::FlushBuffers();
	RenderLine(DstHeight&1 ? XScaledLineB : XScaledLineA, DstHeight);
	DstLastLine = NULL;
}
/*
VOID DitherEngine::SpreadLToR (struct RGBPixel *line, struct RGBPixel *below, LONG br, LONG bg, LONG bb)
{
#ifdef FAST
	(*line).Add(br, bg, bb);
#else
	if(br || bg || bb)
	{
		LONG tr, tg, tb, r, g, b;

		tr = 7*br >> 4; tg = 7*bg >> 4; tb = 7*bb >> 4;
		(*line).Add(tr, tg, tb);
		r = br - tr; g = bg - tg; b = bb - tb;

		tr = 5*br >> 4; tg = 5*bg >> 4; tb = 5*bb >> 4;
		below[-1].Add(tr, tg, tb);
		r -= tr; g -= tg; b -= tb;

		tr = 3*br >> 4; tg = 3*bg >> 4; tb = 3*bb >> 4;
		(*below++).Add(tr, tg, tb);
		r -= tr; g -= tg; b -= tb;

		(*below).Add(r, g, b);
	}
#endif
}

VOID DitherEngine::SpreadRToL (struct RGBPixel *line, struct RGBPixel *below, LONG br, LONG bg, LONG bb)
{
#ifdef FAST
	(*line).Add(br, bg, bb);
#else
	if(br || bg || bb)
	{
		LONG tr, tg, tb, r, g, b;

		tr = 7*br >> 4; tg = 7*bg >> 4; tb = 7*bb >> 4;
		(*line).Add(tr, tg, tb);
		r = br - tr; g = bg - tg; b = bb - tb;

		tr = 5*br >> 4; tg = 5*bg >> 4; tb = 5*bb >> 4;
		below[+1].Add(tr, tg, tb);
		r -= tr; g -= tg; b -= tb;

		tr = 3*br >> 4; tg = 3*bg >> 4; tb = 3*bb >> 4;
		(*below).Add(tr, tg, tb);
		r -= tr; g -= tg; b -= tb;

		(*--below).Add(r, g, b);
	}
#endif
}
*/
