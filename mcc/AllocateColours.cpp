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

#include <proto/graphics.h>

#include "General.h"
#include "Classes.h"
#include "IM_Render.h"

//#define TRACE_ALLOCATIONS

#ifdef TRACE_ALLOCATIONS
LONG AllocatedColours = 0;

VOID __EXIT_7_Colours ()
{
  if(AllocatedColours)
    D(DBF_ALWAYS, "HTMLview: %ld colours left!", AllocatedColours);
}
#endif

/*
LONG AllocPen (struct ColorMap *cmap, LONG t_rgb)
{
#ifdef TRACE_ALLOCATIONS
  AllocatedColours++;
#endif

  if(t_rgb == -1)
    return(-1);

  UBYTE *rgb = (UBYTE *)&t_rgb;
  ULONG r, g, b;
  r = Precision(rgb[1]);
  g = Precision(rgb[2]);
  b = Precision(rgb[3]);

  return(ObtainBestPenA(cmap, r, g, b, NULL));
}

inline VOID FreePen (struct ColorMap *cmap, ULONG pen)
{
  ReleasePen(cmap, pen);
#ifdef TRACE_ALLOCATIONS
  AllocatedColours--;
#endif
}
*/

static inline ULONG mmin(ULONG a,ULONG b) __attribute((always_inline));
static inline ULONG mmax(ULONG a,ULONG b) __attribute((always_inline));

static inline ULONG mmin(ULONG a,ULONG b)
{
	return a>b ? b : a;
}

static inline ULONG mmax(ULONG a,ULONG b)
{
	return a>b ? a : b;
}

VOID ObtainShineShadowPens (struct ColorMap *cmap, ULONG rgb, LONG &shine, LONG &shadow)
{
  ULONG r = (rgb & 0xff0000) >> 16;
  ULONG g = (rgb & 0x00ff00) >> 8;
  ULONG b = rgb & 0x0000ff;

  ULONG dark_r = mmax(64, r)-64;
  ULONG dark_g = mmax(64, g)-64;
  ULONG dark_b = mmax(64, b)-64;

  ULONG bright_r = mmin(191, r)+64;
  ULONG bright_g = mmin(191, g)+64;
  ULONG bright_b = mmin(191, b)+64;

  ULONG intencity = (r+g+b)/3;
  if(intencity >= 215)
  {
    bright_r = mmin(255, dark_r+32);
    bright_g = mmin(255, dark_g+32);
    bright_b = mmin(255, dark_b+32);

    dark_r = mmax(0, (LONG)dark_r-64);
    dark_g = mmax(0, (LONG)dark_g-64);
    dark_b = mmax(0, (LONG)dark_b-64);
  }
  else
  {
    if(intencity < 32)
    {
      dark_r = mmax(0, (LONG)bright_r);
      dark_g = mmax(0, (LONG)bright_g);
      dark_b = mmax(0, (LONG)bright_b);

      bright_r = mmin(255, bright_r+64);
      bright_g = mmin(255, bright_g+64);
      bright_b = mmin(255, bright_b+64);
    }
  }

  shine   = ObtainBestPenA(cmap, Precision(bright_r), Precision(bright_g), Precision(bright_b), NULL);
  shadow  = ObtainBestPenA(cmap, Precision(dark_r), Precision(dark_g), Precision(dark_b), NULL);

/*  ULONG i_brighter = (bright_r << 16) | (bright_g << 8) | bright_b;
  ULONG i_darker = (dark_r << 16) | (dark_g << 8) | dark_b;
  ULONG RGB[3];
  GetRGB32(cmap, shine, 1, RGB);
  ULONG brighter = ((RGB[0] >> 8) & 0xff0000) | ((RGB[1] >> 16) & 0xff00) | ((RGB[2] >> 24) & 0xff);
  GetRGB32(cmap, shadow, 1, RGB);
  ULONG darker = ((RGB[0] >> 8) & 0xff0000) | ((RGB[1] >> 16) & 0xff00) | ((RGB[2] >> 24) & 0xff);
  D(DBF_ALWAYS, "0x%06lx (0x%06lx =? 0x%06lx, 0x%06lx =? 0x%06lx)", rgb, brighter, i_brighter, darker, i_darker);
*/
}
