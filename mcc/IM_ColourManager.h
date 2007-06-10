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

#ifndef COLOURMANAGER_H
#define COLOURMANAGER_H

#define PRECISION(c) ((c)*0x01010101)
#define PIXEL_SIZE 4
#define PIXEL_FORMAT 1
/*
#define RECTFMT_RGB     0
#define RECTFMT_RGBA      1
#define RECTFMT_ARGB      2
#define RECTFMT_LUT8      3
#define RECTFMT_GREY8   4
*/


/* Protos */
extern "C" class ColourManager *ObtainCMap (struct Screen *scr);
VOID ReleaseCMap (class ColourManager *cmap);

inline VOID Round (UBYTE &col, LONG &add)
{
/*  if(add > 0)
  {
    if(col + add > 255)
      add = 255 - col;
  }
  else
  {
    if(col + add < 0)
      add = -col;
  }
  col += add;*/

  WORD sum;
//  col = add > 0 ? (sum > 255 ? 255 : sum) : (sum < 0 ? 0 : sum);
  col = (sum = col+add) & 0xff00 ? (add > 0 ? 255 : 0) : sum;
}

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack(2)
  #elif defined(__VBCC__)
    #pragma amiga-align
  #endif
#endif

struct RGBPixel
{
  UBYTE R, G, B, A;

  VOID Init (ULONG *rgb)
  {
    R = *rgb++ >> 24;
    G = *rgb++ >> 24;
    B = *rgb >> 24;
  }

//  VOID Add (LONG &r, LONG &g, LONG &b)
  VOID Add (LONG r, LONG g, LONG b)
  {
    LONG sum;
    sum = R+r;
    R = r > 0 ? (sum > 255 ? 255 : sum) : (sum < 0 ? 0 : sum);
    sum = G+g;
    G = g > 0 ? (sum > 255 ? 255 : sum) : (sum < 0 ? 0 : sum);
    sum = B+b;
    B = b > 0 ? (sum > 255 ? 255 : sum) : (sum < 0 ? 0 : sum);

/*    R = (sum = R+r) & 0xff00 ? (r > 0 ? 255 : 0) : sum;
    G = (sum = G+g) & 0xff00 ? (g > 0 ? 255 : 0) : sum;
    B = (sum = B+b) & 0xff00 ? (b > 0 ? 255 : 0) : sum;
*/
/*    Round(R, r);
    Round(G, g);
    Round(B, b);
*/  }

  ULONG Index ()
  {
    return((R & 0xe0) | (G & 0xe0) >> 3 | B >> 6);
  }

  ULONG RGB ()
  {
    return(*(ULONG *)&R);
  }

  ULONG Scale (UBYTE factor)
  {
    return(
      (R*factor / 255) << 24 |
      (G*factor / 255) << 16 |
      (B*factor / 255) <<  8);
  }

  VOID SetRGB (ULONG RGBA)
  {
    *(ULONG *)&R = RGBA;
  }

  VOID SetR (LONG r) { R = r > 255 ? 255 : (r < 0 ? 0 : r); }
  VOID SetG (LONG g) { G = g > 255 ? 255 : (g < 0 ? 0 : g); }
  VOID SetB (LONG b) { B = b > 255 ? 255 : (b < 0 ? 0 : b); }
};

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack()
  #elif defined(__VBCC__)
    #pragma default-align
  #endif
#endif

class ColourManager
{
  public:
    ColourManager (struct Screen *scr);
    ~ColourManager ();

    class ColourManager *Next, *Prev;
    LONG Allocated[256];
    struct RGBPixel CValues[256];
    ULONG LockCnt;
    struct ColorMap *ColourMap;
};

#endif
