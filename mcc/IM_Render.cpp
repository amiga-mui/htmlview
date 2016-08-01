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
#include <stdlib.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <clib/macros.h>
#if defined(__amigaos4__)
#include <graphics/blitattr.h>
#endif

#include "ImageManager.h"
#include "IM_ColourManager.h"
#include "IM_Scale.h"
#include "ImageDecoder.h"
#include "General.h"
#include <new>

PictureFrame::PictureFrame (LONG width, LONG height, LONG leftofs, LONG topofs, ULONG animdelay, ULONG disposal, struct RGBPixel *background, struct BitMap *bmp, UBYTE *mask, ULONG flags)
{
  Width = width, Height = height;
  LeftOfs = leftofs, TopOfs = topofs;
  AnimDelay = animdelay;
  Disposal = disposal;
  if(background)
    Background = *background;
  BMp = bmp;
  Mask = mask;
  Flags = flags;

/*  if(GetBitMapAttr(bmp, BMA_FLAGS) & BMF_INTERLEAVED)
      Depth = GetBitMapAttr(bmp, BMA_DEPTH);
  else  */Depth = 1;
}

PictureFrame::~PictureFrame ()
{
  WaitBlit();
  FreeBitMap(BMp);
  if(Mask)
    FreeRaster(Mask, Width, Height * Depth);
  delete AlphaMask;
  ReleaseCMap(CMap);
  delete Next;
}

VOID PictureFrame::LockPicture ()
{
  LockCnt++;
}

VOID PictureFrame::UnLockPicture ()
{
  if(!--LockCnt)
    delete this;
}

ULONG PictureFrame::Size ()
{
  ULONG size;
  /*
  if(CyberGfxBase && GetCyberMapAttr(BMp, CYBRMATTR_ISCYBERGFX))
      size = Height * GetCyberMapAttr(BMp, CYBRMATTR_XMOD);
  else*/  size = GetBitMapAttr(BMp, BMA_DEPTH) * RASSIZE(Width, Height);

  return(size + (Next ? Next->Size() : 0));
}

BOOL PictureFrame::MatchSize (LONG width, LONG height)
{
  return((width ? Width == width : !(Flags & PicFLG_ScaledX)) && (height ? Height == height : !(Flags & PicFLG_ScaledY)));
}

RenderEngine::RenderEngine (struct Screen *scr, struct DecoderData *data)
{
  Scr = scr;
  Data = data;
  TmpRP = scr->RastPort;
  TmpRP.Layer = NULL;
  TmpRP.BitMap = NULL;
  InitRastPort(&BMpRP);
  LastFrame = (struct PictureFrame *)&FirstFrame;
}

RenderEngine::~RenderEngine ()
{
  if(TmpRP.BitMap)
  {
    WaitBlit();
    FreeBitMap(TmpRP.BitMap);
  }
}

VOID RenderEngine::SetCMap (class ColourManager *cmap)
{
  FirstFrame->CMap = cmap;
}

BOOL RenderEngine::AllocateFrame (ULONG width, ULONG height, ULONG animdelay, ULONG disposal, ULONG leftofs, ULONG topofs, struct RGBPixel *background, ULONG transparency, ULONG flags)
{
  struct BitMap *my_bmp, *friend_bmp = Scr->RastPort.BitMap;
  ULONG depth = GetBitMapAttr(friend_bmp, BMA_DEPTH);
  if(!CyberGfxBase && depth > 8)
    depth = 8;

  if((BMpRP.BitMap = my_bmp = AllocBitMap(width, height, depth, BMF_MINPLANES | BMF_CLEAR, friend_bmp)))
  {
    BOOL first = FirstFrame ? FALSE : TRUE;

    Mask = AlphaMask = NULL;
    switch(transparency)
    {
      case TransparencySINGLE:
      {
        ULONG t_height = height;
//        if(GetBitMapAttr(my_bmp, BMA_FLAGS) & BMF_INTERLEAVED)
//          t_height *= depth;
        char buf[32];
        if(GetVar("IP_Solid", buf, 32, 0L) == -1)
        {
          if((Mask = AllocRaster(width, t_height)))
            memset(Mask, 0, RASSIZE(width, t_height));
        }
      }
      break;

      case TransparencyALPHA:
        if(depth >= 15)
        {
          AlphaMask = new (std::nothrow) UBYTE [width * height];
          if (!AlphaMask) return FALSE;
        }
      break;
    }

    if((LastFrame->Next = new (std::nothrow) struct PictureFrame(width, height, leftofs, topofs, animdelay, disposal, background, my_bmp, Mask, flags)))
    {
      LastFrame->Next->AlphaMask = AlphaMask;
      if(depth <= 8)
      {
        if(!first)
        {
          WaitBlit();
          FreeBitMap(TmpRP.BitMap);
        }

        if(!(TmpRP.BitMap = AllocBitMap((width+15) & ~15, 1, 8, NULL, friend_bmp)))
          goto error;
      }

      if(first)
        Data->StatusItem->Start(FirstFrame);

      LastFrame = LastFrame->Next;
      return(TRUE);

error:  delete LastFrame->Next;
      delete AlphaMask;
      LastFrame->Next = NULL;
    }
    else
    {
      if(Mask)
        FreeRaster(Mask, width, /*depth * */height);
      delete AlphaMask;
      FreeBitMap(my_bmp);
    }
  }
  return(FALSE);
}

VOID RenderEngine::Multiply (ULONG width, ULONG y, ULONG height)
{
  height--;

  if(Mask)
  {
    ULONG depth = LastFrame->Depth;
    y *= depth;

    ULONG modulo = RASSIZE(width, 1/*depth*/), c_modulo = RASSIZE(width, 1);
    UBYTE *src = Mask + RASSIZE(width, y);
    for(UBYTE *dst = src+height*modulo; dst > src; dst -= modulo)
      memcpy(dst, src, c_modulo);
  }

  struct BitMap *bmp = BMpRP.BitMap;
  for(ULONG t_h, h = 1; height; h <<= 1)
  {
    height -= t_h = MIN(h, height);
    BltBitMap(bmp, 0, y, bmp, 0, y+h, width, t_h, 0xc0, ~0, NULL);
  }
}

VOID RenderEngine::WriteMask (struct RGBPixel *srcline, ULONG width, ULONG y)
{
  if(Mask)
  {
//    y *= LastFrame->Depth;

    UBYTE *start = Mask + RASSIZE(width, y);
    width = (width+15) >> 3;

    while(--width)
    {
      UBYTE byte = 0;
      for(UWORD i = 128; i; i >>= 1)
      {
        if((*srcline++).A)
          byte |= i;
      }
      *start++ = byte;
    }
  }
  else if(AlphaMask)
  {
    UBYTE *start = AlphaMask + y*width;
    while(width--)
      *start++ = (*srcline++).A;
  }
}

VOID RenderEngine::WriteLine (ULONG width, struct RGBPixel *line, ULONG y)
{
#if defined(__amigaos4__)
  #warning BLITT_ARGB32 is wrong, because we are using RGBA instead of ARGB
  BltBitMapTags(BLITA_Source, line,
                BLITA_Dest, &BMpRP,
                BLITA_SrcX, 0,
                BLITA_SrcY, 0,
                BLITA_DestX, 0,
                BLITA_DestY, y,
                BLITA_Width, width,
                BLITA_Height, 1,
                BLITA_SrcType, BLITT_ARGB32,
                BLITA_DestType, BLITT_RASTPORT,
                BLITA_SrcBytesPerRow, width,
                BLITA_UseSrcAlpha, TRUE,
                TAG_DONE);
#else
  WritePixelArray(
    (APTR)line,                 //    srcRect
    0, 0,                       //    (SrcX,SrcY)
    0,                          //    SrcMod
    &BMpRP,                     //    RastPort
    0, y,                       //    (DestX,DestY)
    width, 1,                   //    (SizeX,SizeY)
    PIXEL_FORMAT);
#endif

  Data->StatusItem->Enter();
  if(Data->StatusItem->CurrentY > y)
    Data->StatusItem->CurrentPass++;
  Data->StatusItem->CurrentY = y+1;
  Data->StatusItem->Leave();
}

VOID RenderEngine::WriteLine (ULONG width, UBYTE *chunky, ULONG y)
{
  WritePixelLine8(&BMpRP, 0, y, width, chunky, &TmpRP);

  Data->StatusItem->Enter();
  if(Data->StatusItem->CurrentY > y)
    Data->StatusItem->CurrentPass++;
  Data->StatusItem->CurrentY = y+1;
  Data->StatusItem->Leave();
}
