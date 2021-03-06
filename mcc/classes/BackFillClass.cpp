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

#include "BackFillClass.h"

#include "ScanArgs.h"
#include "ParseMessage.h"
#include "Layout.h"
#include "SharedData.h"
#include "IM_Render.h"

#include "SDI_hook.h"

#include <proto/layers.h>

BackFillClass::~BackFillClass ()
{
  delete Source;
}

VOID BackFillClass::AllocateColours (struct ColorMap *cmap)
{
  if(!(Flags & FLG_AllocatedColours) && (Flags & FLG_ArgumentsRead))
  {
    BackgroundCol = AllocPen(cmap, BackgroundRGB);
    Flags |= FLG_AllocatedColours;
  }
  TreeClass::AllocateColours(cmap);
}

VOID BackFillClass::FreeColours (struct ColorMap *cmap)
{
  if(Picture)
    Picture->UnLockPicture();
  Picture = NULL;

  if(Flags & FLG_AllocatedColours)
  {
    FreePen(cmap, BackgroundCol);
    Flags &= ~FLG_AllocatedColours;
  }
  TreeClass::FreeColours(cmap);
}

VOID BackFillClass::GetImages (struct GetImagesMessage &gmsg)
{
  if(Source && !Picture)
  {
    STRPTR url;
    if((url = (STRPTR)DoMethod(gmsg.HTMLview, MUIM_HTMLview_AddPart, (ULONG)Source)))
      gmsg.AddImage(url, 0, 0, this);
  }
  TreeClass::GetImages(gmsg);
}

VOID BackFillClass::FindImage (struct LayoutMessage &lmsg)
{
  STRPTR url;
  if(!Picture && Source && (url = (STRPTR)DoMethod(lmsg.HTMLview, MUIM_HTMLview_AddPart, (ULONG)Source)))
  {
    if((Picture = lmsg.Share->ImageStorage->FindImage(url, 0, 0)))
      Picture->LockPicture();

    delete url;
  }
}

VOID BackFillClass::Parse(struct ParseMessage &pmsg)
{
  struct ArgList args[] =
  {
    { "BACKGROUND", &Source,        ARG_URL,    NULL  },
    { "BGCOLOR",    &BackgroundRGB, ARG_COLOUR, NULL  },
    { NULL,         NULL,           0,          NULL  }
  };
  ScanArgs(pmsg.Locked, args);
  TreeClass::Parse(pmsg);
}

BOOL BackFillClass::ReceiveImage (struct PictureFrame *pic)
{
  Picture = pic;
  pic->LockPicture();
  return(FALSE);
}

BOOL BackFillClass::UpdateImage(LONG ystart UNUSED, LONG ystop UNUSED, LONG top UNUSED, LONG bottom UNUSED, BOOL last)
{
  return(Picture ? last : FALSE);
}

BOOL BackFillClass::FlushImage(LONG top UNUSED, LONG bottom UNUSED)
{
  if(Picture)
  {
    Picture->UnLockPicture();
    Picture = NULL;
    return(TRUE);
  }
  return(FALSE);
}

BOOL BackFillClass::ReadyForAlpha ()
{
  return(!Source || (Picture && Picture->Flags & PicFLG_Complete));
}

BOOL BackFillClass::Complex ()
{
  return(Picture && Picture->Flags & PicFLG_Complete);
}

struct LayerMsg
{
  struct Layer *layer;
  struct Rectangle bounds;
  LONG offsetx, offsety;
};

struct BackfillInfo
{
  struct PictureFrame *Picture;
  LONG left, top;
  LONG subleft, subtop;
};

HOOKPROTO(BackFillCode, VOID, struct RastPort *rp, struct LayerMsg *lmsg)
{
  struct BackfillInfo *info = (struct BackfillInfo *)hook->h_Data;
  struct PictureFrame *pic = info->Picture;

  WORD minx, miny, maxx, maxy;
  minx = lmsg->bounds.MinX;
  miny = lmsg->bounds.MinY;
  maxx = lmsg->bounds.MaxX;
  maxy = lmsg->bounds.MaxY;

  LONG offx = lmsg->offsetx - info->subleft;
  LONG offy = lmsg->offsety - info->subtop;

  ULONG width, height, bmp_width = pic->Width, bmp_height = pic->Height;
  LONG xoffset = (info->left+offx) % bmp_width;
  LONG yoffset = (info->top+offy)  % bmp_height;
  LONG srcx = xoffset, srcy = yoffset;
  struct BitMap *ysrc = pic->BMp, *dst = rp->BitMap;
  bmp_width -= xoffset;
  bmp_height -= yoffset;
  for(UWORD y = miny; y <= maxy; y += height)
  {
    height = min((ULONG)maxy-y+1, bmp_height);
    struct BitMap *src = ysrc;
    for(UWORD x = minx; x <= maxx; x += width)
    {
      width = min((ULONG)maxx-x+1, bmp_width);
      BltBitMap(src, srcx, srcy, dst, x, y, width, height, 0x0c0, ~0, NULL);

      if(src == dst)
      {
        bmp_width *= 2;
      }
      else if(x == minx && srcx)
      {
        srcx = 0;
        bmp_width = pic->Width;
      }
      else
      {
        src = dst;
        srcx = x;
        srcy = y;
        bmp_width = pic->Width;
      }
    }

    if(ysrc == dst)
    {
      bmp_height *= 2;
    }
    else if(y == miny && yoffset)
    {
      srcx = xoffset;
      srcy = yoffset = 0;
      bmp_width = pic->Width - xoffset;
      bmp_height = pic->Height;
    }
    else
    {
      ysrc = dst;
      srcx = minx;
      srcy = y;
      bmp_height = pic->Height;
    }
  }
}
//MakeStaticCppHook(BackFillHook, BackFillCode);

VOID BackFillClass::DrawBackground (struct RenderMessage &rmsg, LONG minx, LONG miny, LONG maxx, LONG maxy, LONG left, LONG top)
{
  struct RastPort *rp = rmsg.RPort;

  if(Picture && (Picture->Flags & PicFLG_Complete))
  {
    struct BackfillInfo info = { Picture, left, top, minx, miny };
    struct Rectangle rect = { minx, miny, maxx, maxy };
    MakeCppHook(BackFillHook,BackFillCode);

	BackFillHook.h_Data = &info;
    //InitHook(&BackFillHook, BackFillHook, &info);

    DoHookClipRects(&BackFillHook, rp, &rect);
  }
  else
  {
    SetAPen(rp, rmsg.Colours[Col_Background]);
    if(minx <= maxx && miny <= maxy)
      RectFill(rp, minx, miny, maxx, maxy);
  }
}
