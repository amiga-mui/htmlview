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

#include <clib/alib_protos.h>
#include <clib/macros.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include "Animation.h"
#include "IM_Render.h"
#include "IM_ColourManager.h"
#include "private.h"
//#include "Data:C-Sources/ImageRender/Decoders/ImageDecoder.h"
#include "ImageManager.h" /* struct DecodeItem */

#include "classes/SuperClass.h"

enum
{
  DisposeUNKNOWN = 0,
  DisposeNOP,
  DisposeBACKGROUND,
  DisposePREVIOUS
};


VOID InitMaskBitMap (struct BitMap &bmp, PLANEPTR mask, ULONG w, ULONG h, ULONG d)
{
  memset(&bmp, 0, sizeof(struct BitMap));
  InitBitMap(&bmp, d, w, h);
  if(d > 1)
    bmp.Flags |= BMF_INTERLEAVED;

  for(UWORD i = 0; i < d; i++)
    bmp.Planes[i] = mask + i*RASSIZE(w, 1);
}

AnimInfo::AnimInfo (Object *obj, struct HTMLviewData *data, struct PictureFrame *pic, struct ObjectList *receivers, struct AnimInfo *next)
{
  Obj = obj;
  Data = data;
  Next = next;
  FirstFrame = CurrentFrame = pic;
  Receivers = receivers;
  TimeLeft = pic->AnimDelay;
  InitRastPort(&BMpRP);

  pic->LockPicture();

  ULONG width = pic->Width, height = pic->Height, depth = pic->Depth;

  BMp = pic->BMp;
  do {
    pic->BMp = BMpRP.BitMap = AllocBitMap(width, height, GetBitMapAttr(BMp, BMA_DEPTH), BMF_MINPLANES, BMp);
  } while(!pic->BMp);
  BltBitMap(BMp, 0, 0, BMpRP.BitMap, 0, 0, width, height, 0xc0, ~0, NULL);

  if((Mask = pic->Mask))
  {
    depth = 1;
    do {
      pic->Mask = AllocRaster(width, depth * height);
    } while (!pic->Mask);
    memcpy(pic->Mask, Mask, RASSIZE(width, depth * height));

    InitMaskBitMap(MaskBMp, pic->Mask, width, height, depth);
    InitRastPort(&MaskRP);
    MaskRP.BitMap = &MaskBMp;
    SetAPen(&MaskRP, 0);
  }
}

AnimInfo::~AnimInfo ()
{
  WaitBlit();
  FreeBitMap(FirstFrame->BMp);
  if(FirstFrame->Mask)
    FreeRaster(FirstFrame->Mask, FirstFrame->Width, /*FirstFrame->Depth * */FirstFrame->Height);

  FirstFrame->BMp = BMp;
  FirstFrame->Mask = Mask;
  FirstFrame->UnLockPicture();

  if(Decode)
    Decode->Anim = NULL;

  if(Flags & AnimFLG_DeleteObjList)
    delete Receivers;
}

VOID AnimInfo::Stop (Object *obj, struct AnimInfo *prev)
{
  if(this)
  {
    if(Obj == obj)
    {
      prev->Next = Next;
      delete this;
    }
    else
    {
      prev = this;
    }
    prev->Next->Stop(obj, prev);
  }
}

ULONG AnimInfo::Tick ()
{
  struct DateStamp thistime;
  DateStamp(&thistime);
  LONG minute = thistime.ds_Minute - LastTime.ds_Minute;
  LONG tick = thistime.ds_Tick - LastTime.ds_Tick;
  LastTime = thistime;

  if(minute < 0)
    minute += 24*60;
  tick += minute * 60*TICKS_PER_SECOND;
  tick *= 100/TICKS_PER_SECOND;

  ULONG newtime = 0xfffffff;
  struct AnimInfo *first = this;
  while(first)
  {
    newtime = min(newtime, first->Update(tick));
    first = first->Next;
  }

  DateStamp(&thistime);
  minute = thistime.ds_Minute - LastTime.ds_Minute;
  tick = thistime.ds_Tick - LastTime.ds_Tick;

  if(minute < 0)
    minute += 24*60;
  tick += minute * 60*TICKS_PER_SECOND;
  tick *= 100/TICKS_PER_SECOND;
  if((LONG)newtime < tick)
    tick = newtime;

  return(10*(newtime-tick));
}

#define MINTERM_B_OR_C      (ABC | ABNC | NABC | NABNC | ANBC | NANBC)

/*VOID mBltBitMap (struct BitMap *src, WORD src_x, WORD src_y, struct BitMap *dst, WORD l, WORD t, WORD w, WORD h, UBYTE minterm, UBYTE mask, PLANEPTR storage)
{
  ULONG src_w, src_h;
  src_w = GetBitMapAttr(src, BMA_WIDTH);
  src_h = GetBitMapAttr(src, BMA_HEIGHT);
  ULONG dst_w, dst_h;
  dst_w = GetBitMapAttr(dst, BMA_WIDTH);
  dst_h = GetBitMapAttr(dst, BMA_HEIGHT);

  if(src_x+w <= src_w && src_y+h <= src_h && l+w <= dst_w && t+h <= dst_h)
      BltBitMap(src, src_x, src_y, dst, l, t, w, h, minterm, mask, storage);
  else  kprintf("*** Error: (%ld, %ld) %ld, %ld - (%ld, %ld) %ld, %ld, %ld, %ld\n", src_w, src_h, src_x, src_y, dst_w, dst_h, l, t, w, h);
}

VOID mRectFill (struct RastPort *rp, WORD l, WORD t, WORD r, WORD b)
{
  ULONG w, h;
  w = GetBitMapAttr(rp->BitMap, BMA_WIDTH);
  h = GetBitMapAttr(rp->BitMap, BMA_HEIGHT);

  if(r < w && b < h)
      RectFill(rp, l, t, r, b);
  else  kprintf("*** Error: (%ld, %ld) %ld, %ld, %ld, %ld\n", w, h, l, t, r, b);
}*/

#define mBltBitMap BltBitMap
#define mRectFill RectFill

ULONG AnimInfo::Update (ULONG diff)
{
  if(0 >= (LONG)(TimeLeft -= diff))
  {
    if(!Decode || (CurrentFrame->Next && CurrentFrame->Next->Next))
    {
      LONG dst_w, dst_h;
      dst_w = GetBitMapAttr(BMpRP.BitMap, BMA_WIDTH);
      dst_h = GetBitMapAttr(BMpRP.BitMap, BMA_HEIGHT);

      BOOL copy_mask = FALSE, or_mask = FALSE;
      LONG left = CurrentFrame->LeftOfs, top = CurrentFrame->TopOfs, width = CurrentFrame->Width, height = CurrentFrame->Height;
      LONG bottom = 0;
//      if(CurrentFrame != FirstFrame)
      {
        switch(CurrentFrame->Disposal)
        {
          case DisposeNOP:
            or_mask = TRUE;
          break;

          case DisposeBACKGROUND:
          {
            if(dst_w < left+width || dst_h < top+height)
            {
              width = MIN(width, dst_w-left);
              height = MIN(height, dst_h-top);
              if(width <= 0 || height <= 0)
                break;
            }

            copy_mask = TRUE;
            if(Mask)
              mRectFill(&MaskRP, left, top, left+width-1, top+height-1);

            SetAPen(&BMpRP, Data->RenderMsg.Colours[Col_Background]);
            mRectFill(&BMpRP, left, top, left+width-1, top+height-1);

            bottom = top+height;
          }
          break;

          case DisposePREVIOUS:
            copy_mask = TRUE;
//            kprintf("Unsupported dispose method\n");
//            BltBitMapRastPort(FirstFrame->BMp, left, top, Win->RPort, Win->BorderLeft + left, Win->BorderTop + top, width, height, 0xc0);
          break;
        }
      }

      UBYTE *mask;
      struct BitMap *bmp;
      if((CurrentFrame = CurrentFrame->Next))
      {
        bmp = CurrentFrame->BMp;
        mask = CurrentFrame->Mask;
      }
      else
      {
        CurrentFrame = FirstFrame;
        bmp = BMp;
        mask = Mask;
      }

      LONG l = CurrentFrame->LeftOfs, t = CurrentFrame->TopOfs, w = CurrentFrame->Width, h = CurrentFrame->Height;
      LONG o_w = w, o_h = h;

      if(dst_w < l+w || dst_h < t+h)
      {
        l = MIN(l, dst_w-1);
        t = MIN(t, dst_h-1);
        w = MAX(0, MIN(w, dst_w-l));
        h = MAX(0, MIN(h, dst_h-t));
      }

      if(mask)
      {
        if(FirstFrame->Mask && copy_mask)
        {
//          ULONG minterm = (w == FirstFrame->Width && h == FirstFrame->Height) ? 0xc0 : MINTERM_B_OR_C;
          ULONG minterm = 0xc0;

          struct BitMap src;
          InitMaskBitMap(src, mask, o_w, o_h, 1 /*FirstFrame->Depth*/);

          mBltBitMap(&src, 0, 0, &MaskBMp, l, t, w, h, minterm, 1, NULL);
        }

        if(FirstFrame->Mask && or_mask)
        {
          ULONG minterm = MINTERM_B_OR_C;
          struct BitMap src;
          InitMaskBitMap(src, mask, o_w, o_h, 1 /*FirstFrame->Depth*/);
          mBltBitMap(&src, 0, 0, &MaskBMp, l, t, w, h, minterm, 1, NULL);
        }

/*        SetAPen(&BMpRP, 3);
        BltTemplate(mask, 0, RASSIZE(dst_w, 1), &BMpRP, l, t, w, h);
*/        BltMaskRPort(bmp, 0, 0, &BMpRP, l, t, w, h, mask);
      }
      else
      {
        mBltBitMap(bmp, 0, 0, BMpRP.BitMap, l, t, w, h, 0xc0, ~0, NULL);
      }

      bottom = MAX(bottom, t+h);
      t = MIN(t, top);

/*      if(t > top)
      {
        h += t-top;
        t = top;
      }
      if(h < height)
        h = height;
*/
      struct ObjectList *first = Receivers;
      while(first)
      {
        if(first->Obj->id() != tag_BODY)
        {
          if(first->Obj->UpdateImage(t, bottom, Data->Top, Data->Top+Data->Height-1, TRUE))
          {
            Data->RedrawObj = first->Obj;
            MUI_Redraw(Obj, MADF_DRAWUPDATE);
          }
        }
        first = first->Next;
      }
    }
    TimeLeft = CurrentFrame->AnimDelay;
  }
  return(TimeLeft);
}
