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

#include <string.h>
#include <stdio.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include "IM_ColourManager.h"
#include "IM_Scale.h"
#include "General.h"
#include "ImageDecoder.h"

ScaleEngine::ScaleEngine (struct Screen *scr, ULONG width, ULONG height, struct DecoderData *data) : RenderObj(scr, data)
{
	Scr = scr;
	DstWidth = width;
	DstHeight = height;
}

ScaleEngine::~ScaleEngine ()
{
	delete SrcBuffer;
	delete ScaleBuffer;
	delete DstBuffer;
}

struct RGBPixel *ScaleEngine::GetLineBuffer (ULONG y, BOOL lastpass)
{
	struct RGBPixel *dst = y&1 ? SrcLineB : SrcLineA;

	LastPass = lastpass;
	if(Interlaced)
	{
		LONG modulo = SrcWidth+3;
		if(Interlaced == InterlaceADAM7)
		{
			dst = SrcLineA + (y*modulo);
		}
		else
		{
			BOOL scaley = SrcHeight < DstHeight;
			if(Dither || scaley)
			{
				dst = SrcLineA + modulo*(y/2 + 1);
				if(lastpass && y)
				{
					DrawLineBuffer(dst, y-1, 1);
					dst -= modulo;
				}
			}
		}
	}
	return(dst);
}

BOOL ScaleEngine::SetDimensions (ULONG width, ULONG height, BOOL interlaced, ULONG animdelay, ULONG disposal, ULONG leftofs, ULONG topofs, struct RGBPixel *background, ULONG transparency)
{
	SrcWidth = width;
	SrcHeight = height;
	Interlaced = interlaced;

	if(NotFirstFrame)
	{
		FlushBuffers();
		delete SrcBuffer;
		delete ScaleBuffer;
		delete DstBuffer;
		SrcBuffer = ScaleBuffer = DstBuffer = NULL;

		if(OrgWidth > 1)	leftofs = leftofs * (ScaleWidth-1) / (OrgWidth-1);
		if(OrgHeight > 1)	topofs = topofs * (ScaleHeight-1) / (OrgHeight-1);
		DstWidth	= SrcWidth*ScaleWidth / OrgWidth;
		DstHeight = SrcHeight*ScaleHeight / OrgHeight;
	}
	else
	{
		NotFirstFrame = TRUE;
		if(!DstWidth)	DstWidth = SrcWidth;
		if(!DstHeight)	DstHeight = SrcHeight;

		ScaleWidth = DstWidth;
		ScaleHeight = DstHeight;
		OrgWidth = SrcWidth;
		OrgHeight = SrcHeight;
	}

	ULONG modulo = SrcWidth+3;

	ULONG src_size;
	if(interlaced == InterlaceADAM7)
			src_size = modulo*SrcHeight;
	else if(interlaced && (Dither || SrcHeight < DstHeight))
			src_size = modulo*((SrcHeight+3) / 2); // Half + 1
	else	src_size = modulo*2;
	SrcBuffer = new RGBPixel [src_size];

	BOOL result;
	if((result = SrcBuffer ? TRUE : FALSE))
	{
		DstLineA = XScaledLineA = SrcLineA = SrcBuffer+2;
		DstLineB = XScaledLineB = SrcLineB = SrcLineA + modulo;

		if(SrcWidth != DstWidth)
		{
			modulo = DstWidth+3;
			ScaleBuffer = new RGBPixel [modulo*2];
			DstLineA = XScaledLineA = ScaleBuffer+2;
			DstLineB = XScaledLineB = XScaledLineA + modulo;

			if(!ScaleBuffer)
				result = FALSE;
		}

		DstBuffer = NULL;
		if(SrcHeight != DstHeight)
		{
			DstBuffer = new RGBPixel [modulo*2];
			DstLineA = DstBuffer+2;
			DstLineB = DstLineA + modulo;

			if(!DstBuffer)
				result = FALSE;
		}

		ULONG flags = (SrcWidth != DstWidth ? PicFLG_ScaledX : 0) | (SrcHeight != DstHeight ? PicFLG_ScaledY : 0);
		if(!RenderObj.AllocateFrame(DstWidth, DstHeight, animdelay, disposal, leftofs, topofs, background, transparency, flags))
			result = FALSE;
	}
	return(result);
}

VOID ScaleEngine::DrawLineBuffer (struct RGBPixel *srcline, ULONG y, ULONG height)
{
	if(SrcWidth != DstWidth)
		srcline = ScaleX(srcline, y);

	if(!LastPass)
	{
		LONG dsty = y;
		if(SrcHeight != DstHeight)
		{
			dsty = y*DstHeight / SrcHeight;
			LONG limit = (y+height)*DstHeight / SrcHeight;
			height = limit-dsty;
		}

		if(height)
		{
			RenderObj.WriteMask(srcline, DstWidth, dsty);
			RenderLineND(srcline, dsty);
			RenderObj.Multiply(DstWidth, dsty, min(height, DstHeight-dsty));
		}
	}
	else if(SrcHeight == DstHeight)
	{
		RenderObj.WriteMask(srcline, DstWidth, y);
		RenderLine(srcline, y);
	}
	else if(y--)
	{
		ULONG dsty = y*(DstHeight-1) / (SrcHeight-1);
		ULONG limit = (y+1)*(DstHeight-1) / (SrcHeight-1);

		struct RGBPixel *top		= SrcLastLine;
		struct RGBPixel *bottom	= srcline;

		LONG cnt = 2, delta = limit-dsty + 1;
		while(++dsty <= limit)
		{
			struct RGBPixel *t_dst, *dstline = t_dst = dsty&1 ? DstLineB : DstLineA;
			for(UWORD x = 0; x < DstWidth; x++)
			{
				LONG r = bottom[x].R - top[x].R;
				LONG g = bottom[x].G - top[x].G;
				LONG b = bottom[x].B - top[x].B;
				(*t_dst  ).SetR(top[x].R + (r*cnt / delta));
				(*t_dst  ).SetG(top[x].G + (g*cnt / delta));
				(*t_dst  ).SetB(top[x].B + (b*cnt / delta));
				(*t_dst++).A = bottom[x].A;
			}

			cnt++;
			RenderObj.WriteMask(dstline, DstWidth, dsty);
			RenderLine(dstline, dsty);
		}
	}
	else
	{
		ULONG height = (SrcHeight == 1 ? DstHeight : 1);
		while(height--)
		{
			struct RGBPixel *line = y&1 ? DstLineB : DstLineA;
			memcpy(line, srcline, PIXEL_SIZE*DstWidth);
			RenderObj.WriteMask(line, DstWidth, ++y);
			RenderLine(line, y);
		}
	}
	SrcLastLine = srcline;
}

struct RGBPixel *ScaleEngine::ScaleX (struct RGBPixel *srcline, ULONG y)
{
	struct RGBPixel *dstline = y&1 ? XScaledLineB : XScaledLineA;

	if(DstWidth < SrcWidth || SrcWidth == 1)
	{
		for(UWORD x = 0; x < DstWidth; x++)
			dstline[x] = srcline[x*SrcWidth / DstWidth];
	}
	else
	{
		struct RGBPixel left, right, *line = dstline;
		LONG nsrcx, cnt = 1, delta = 0, r, g, b;
		for(UWORD x = 0; x < DstWidth; x++)
		{
			if(cnt >= delta)
			{
				nsrcx = x*(SrcWidth-1) / (DstWidth-1);
				cnt = 1;
				delta = ((nsrcx+1)*(DstWidth-1) + SrcWidth-2) / (SrcWidth-1) - x;
				delta++;

				left = srcline[nsrcx];
				right = srcline[nsrcx+1];

				/* Why does this happen? */
				if(nsrcx+1 >= SrcWidth)
					right = left;

				r = right.R - left.R;
				g = right.G - left.G;
				b = right.B - left.B;
			}

			(*line).SetR(left.R + (r*cnt / delta));
			(*line).SetG(left.G + (g*cnt / delta));
			(*line).SetB(left.B + (b*cnt++ / delta));
			(*line++).A = left.A;
		}
	}
	return(dstline);
}

VOID ScaleEngine::FlushBuffers ()
{
	if(Interlaced == InterlaceNORMAL && SrcHeight&1 && (Dither || SrcHeight < DstHeight))
	{
		ULONG y = SrcHeight;
		DrawLineBuffer(SrcLineA + (SrcWidth+3)*(y/2 + 1), y-1, 1);
	}
}
