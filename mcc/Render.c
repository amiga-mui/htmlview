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
#include <clib/alib_protos.h>
#include <graphics/gfxmacros.h>
#include <libraries/mui.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/layers.h>

#include "General.h"
#include "Classes.h"
#include "Render.h"
#include "IM_Render.h"
#include "SharedData.h"

#include "private.h"

#define MINTERM_B_OR_C      (ABC | ABNC | NABC | NABNC | ANBC | NANBC)
#define MINTERM_B_EQUALS_C  (ABC | ANBNC | NABC | NANBNC)

struct RastPort *RenderMessage::ObtainDoubleBuffer (ULONG width, ULONG height)
{
	if(BufferRP)
	{
		if(width > BufferWidth || height > BufferHeight)
		{
			WaitBlit();
			FreeBitMap(BufferRP->BitMap);
			BufferWidth = max(BufferWidth, width);
			BufferHeight = max(BufferHeight, height);
			BufferRP->BitMap = AllocBitMap(BufferWidth, BufferHeight, GetBitMapAttr(RPort->BitMap, BMA_DEPTH), BMF_MINPLANES, RPort->BitMap);
		}
	}
	else
	{
		BufferRP = new struct RastPort;
		InitRastPort(BufferRP);
		BufferRP->BitMap = AllocBitMap(width, height, GetBitMapAttr(RPort->BitMap, BMA_DEPTH), BMF_MINPLANES, RPort->BitMap);
		BufferWidth = width;
		BufferHeight = height;
	}
	return(BufferRP->BitMap ? BufferRP : NULL);
}

VOID RenderMessage::FlushDoubleBuffer ()
{
	if(BufferRP)
	{
		WaitBlit();
		FreeBitMap(BufferRP->BitMap);
		delete BufferRP;
		BufferRP = NULL;
	}
}

VOID BltMaskRPort (struct BitMap *source, WORD srcLeft,WORD srcTop, struct RastPort *destination, WORD dstLeft,WORD dstTop,WORD dstWidth,WORD dstHeight, PLANEPTR maskPlane)
{
	if(GetBitMapAttr(source, BMA_FLAGS) & BMF_INTERLEAVED)
	{
		struct BitMap mask;

		InitBitMap(&mask, 8, GetBitMapAttr(source, BMA_WIDTH), GetBitMapAttr(source, BMA_HEIGHT));

		for(UWORD i = 0; i < 8; i++)
			 mask.Planes[i] = maskPlane;

		BltBitMapRastPort(source, srcLeft, srcTop, destination, dstLeft, dstTop, dstWidth, dstHeight, MINTERM_B_EQUALS_C);
		BltBitMapRastPort(&mask, srcLeft,srcTop, destination, dstLeft, dstTop, dstWidth, dstHeight, MINTERM_B_OR_C);
		BltBitMapRastPort(source, srcLeft, srcTop, destination, dstLeft, dstTop, dstWidth, dstHeight, MINTERM_B_EQUALS_C);
	}
	else
	{
		BltMaskBitMapRastPort(source, srcLeft, srcTop, destination, dstLeft, dstTop, dstWidth, dstHeight, (ABC | ABNC | ANBC), maskPlane);
	}
}

VOID RenderMessage::Reset (ULONG minx, ULONG miny, ULONG maxx, ULONG maxy, LONG offsetx, LONG offsety, LONG left, LONG top, LONG *pens)
{
	MinX = minx;
	MinY = miny;
	MaxX = maxx;
	MaxY = maxy;
	OffsetX = offsetx;
	OffsetY = offsety;
	Left = left;
	Top = top;

	UL_Nesting = Textstyles = 0;
	Align = Align_None;

	Colours[Col_Background]	= MUIPEN(pens[Col_Background]);
	Colours[Col_Text]			= MUIPEN(pens[Col_Text]);
	Colours[Col_Shine]		= MUIPEN(pens[Col_Shine]);
	Colours[Col_Shadow]		= MUIPEN(pens[Col_Shadow]);
	Colours[Col_Halfshine]	= MUIPEN(pens[Col_Halfshine]);
	Colours[Col_Halfshadow]	= MUIPEN(pens[Col_Halfshadow]);
	Colours[Col_Link]			= MUIPEN(pens[Col_Link]);
	Colours[Col_VLink]		= MUIPEN(pens[Col_VLink]);
	Colours[Col_ALink]		= MUIPEN(pens[Col_ALink]);

	TargetObj = NULL;
}

#warning "replace own TextLength/TextFit ASAP!!"

LONG MyTextLength (struct TextFont* font, STRPTR string, LONG count)
{
  struct RastPort rp;
  InitRastPort(&rp);
  SetFont(&rp,font);
  return TextLength(&rp, string, count);
}

LONG MyTextFit(struct TextFont *font, char *text, long length, long width, UNUSED long direction)
{
  long res = 0;

  if(length > 0)
  {
    if(font->tf_Flags & FPF_PROPORTIONAL)
    {
      int  stringlength = 0;
      long charsthatfit = length;
      short *spacing = ((short *)font->tf_CharSpace) - font->tf_LoChar;
      short *kerning = ((short *)font->tf_CharKern) - font->tf_LoChar;
      unsigned char current;

      length++;

      while((stringlength < width) && (--length) > 0)
      {
        current = *text;
        stringlength += spacing[current] + kerning[current];

        text++;
      }

      res = charsthatfit-length;
    }
    else
    {
      if((width / font->tf_XSize) < length)
        res = width / font->tf_XSize;
      else
        res = length;
    }
  }

  return res;
}
