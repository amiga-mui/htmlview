/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005 by TextEditor.mcc Open Source Team

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

#ifndef OUTPUT_H
#define OUTPUT_H

#include "IM_ColourManager.h"
#include "IM_Dither.h"

class TrueColourEngine : virtual public ScaleEngine
{
	public:
		TrueColourEngine (struct Screen *scr, ULONG width, ULONG height, struct DecoderData *data)
      : ScaleEngine(scr, width, height, data)
		{
		}

		virtual VOID RenderLine (struct RGBPixel *dstline, ULONG y);
		virtual VOID RenderLineND (struct RGBPixel *dstline, ULONG y) { TrueColourEngine::RenderLine(dstline, y); }
};
/*
class HighColourEngine : public TrueColourEngine, public DitherEngine
{
	public:
		HighColourEngine (struct Screen *scr, ULONG width, ULONG height, struct DecoderData *data) : DitherEngine(scr, width, height, data), TrueColourEngine(scr, width, height, data)
		{
		}

		virtual BOOL SetDimensions (ULONG width, ULONG height, BOOL interlaced, ULONG animdelay, ULONG disposal, ULONG leftofs, ULONG topofs, struct RGBPixel *background, ULONG transparency) { Dither = TRUE; return(TrueColourEngine::SetDimensions(width, height, interlaced, animdelay, disposal, leftofs, topofs, background, transparency)); }
		virtual VOID RenderLine (struct RGBPixel *dstline, ULONG y);
		virtual VOID RenderLineND (struct RGBPixel *dstline, ULONG y) { TrueColourEngine::RenderLine(dstline, y); }
		virtual VOID FlushBuffers () { DitherEngine::FlushBuffers(); }
};
*/
class LowColourNDEngine : virtual public ScaleEngine
{
	public:
		LowColourNDEngine (struct Screen *scr, ULONG width, ULONG height, struct DecoderData *data)
      : ScaleEngine(scr, width, height, data)
		{
		}

		virtual ~LowColourNDEngine ();
		virtual BOOL SetDimensions (ULONG width, ULONG height, BOOL interlaced, ULONG animdelay, ULONG disposal, ULONG leftofs, ULONG topofs, struct RGBPixel *background, ULONG transparency);
		virtual VOID RenderLine (struct RGBPixel *dstline, ULONG y);
		virtual VOID RenderLineND (struct RGBPixel *dstline, ULONG y) { LowColourNDEngine::RenderLine(dstline, y); }

	protected:
		class ColourManager *CMap;
		UBYTE *ChunkyLine;
};

class LowColourEngine : public LowColourNDEngine, public DitherEngine
{
	public:
		LowColourEngine(struct Screen *scr, ULONG width, ULONG height, struct DecoderData *data)
      : ScaleEngine(scr, width, height, data),
        LowColourNDEngine(scr, width, height, data),
        DitherEngine(scr, width, height, data)
		{ }

		virtual BOOL SetDimensions (ULONG width, ULONG height, BOOL interlaced, ULONG animdelay, ULONG disposal, ULONG leftofs, ULONG topofs, struct RGBPixel *background, ULONG transparency) { Dither = TRUE; return(LowColourNDEngine::SetDimensions(width, height, interlaced, animdelay, disposal, leftofs, topofs, background, transparency)); }
		virtual VOID RenderLine (struct RGBPixel *dstline, ULONG y);
		virtual VOID RenderLineND (struct RGBPixel *dstline, ULONG y) { LowColourNDEngine::RenderLine(dstline, y); }
		virtual VOID FlushBuffers () { DitherEngine::FlushBuffers(); }
};

#endif
