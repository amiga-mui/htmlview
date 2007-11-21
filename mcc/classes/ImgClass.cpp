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

#include "ImgClass.h"
#include "TreeClass.h"
#include "MapClass.h"
#include "BackFillClass.h"

#include "IM_Render.h"
#include "Layout.h"
#include "Map.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"
#include "SharedData.h"

#include <proto/cybergraphics.h>

ImgClass::~ImgClass ()
{
  delete Name;
  delete AltText;
  delete Source;
  delete GivenWidth;
  delete GivenHeight;
  delete Map;
}

VOID ImgClass::FreeColours(struct ColorMap *cmap UNUSED)
{
  if(Picture)
    Picture->UnLockPicture();
  Picture = NULL;

  if(BlendBitMap)
  {
    WaitBlit();
    FreeBitMap(BlendBitMap);
    BlendBitMap = NULL;
  }
}

VOID ImgClass::GetImages (struct GetImagesMessage &gmsg)
{
  if(Source && !Picture)
  {
    STRPTR url;
    if((url = (STRPTR)DoMethod(gmsg.HTMLview, MUIM_HTMLview_AddPart, (ULONG)Source)))
      gmsg.AddImage(url, Width(0), Height(0), this);
  }
}

BOOL ImgClass::HitTest (struct HitTestMessage &hmsg)
{
  if(!(Flags & FLG_Layouted))
    return(FALSE);

  BOOL result = FALSE;
  LONG top = Top+VSpace+ImgBorder;
   LONG left = Left+HSpace+ImgBorder;

  if(hmsg.X >= left && hmsg.X < left+Width() && hmsg.Y >= top && hmsg.Y < top+Height())
  {
    hmsg.ImgSrc = Source;
    hmsg.Img = this;
    hmsg.OffsetX = hmsg.X - left;
    hmsg.OffsetY = hmsg.Y - top;

    if(Map)
    {
      if(!MapObj)
        MapObj = hmsg.Host->FindMap(Map);

      if(MapObj)
      {
        struct UseMapMessage umsg(hmsg.X - left, hmsg.Y - top);
        if((result = MapObj->UseMap(umsg)))
        {
          hmsg.Obj = this;
          hmsg.URL = umsg.URL;
          hmsg.Target = umsg.Target;
        }
      }
    }
    else
    {
      if((result = hmsg.Obj ? TRUE : FALSE))
      {
        if(Flags & FLG_Img_IsMap)
        {
          hmsg.ServerMap = TRUE;
        }
        else
        {
          if(Picture && Picture->Mask)
          {
            UBYTE *srcline = Picture->Mask + RASSIZE(Picture->Width, hmsg.Y-top);
            LONG x = hmsg.X-left;
            if(!(result = TestPixel(srcline, x)))
            {
              BOOL left = FALSE, right = FALSE;
              for(LONG lx = x-1; lx > x-5 && lx > 0; lx--)
                left |= TestPixel(srcline, lx);
              for(LONG rx = x+1; rx < x+5 && rx < Picture->Width; rx++)
                right |= TestPixel(srcline, rx);
              result = left && right;
            }
          }
        }
      }
    }
  }
  return(result);
}

VOID ImgClass::Relayout (BOOL all)
{
  if((GivenWidth && GivenWidth->Type == Size_Percent) || (GivenHeight && GivenHeight->Type == Size_Percent))
  {
    /* This will free any scaled bitmaps */
    FreeColours(NULL);
  }
  SuperClass::Relayout(all);
}

LONG ImgClass::Width (LONG def_w, struct LayoutMessage *lmsg)
{
  if(GivenWidth)
  {
    switch(GivenWidth->Type)
    {
      case Size_Percent:
      {
        if(lmsg)
        {
          GivenWidth->Type = Size_Pixels;
          GivenWidth->Size = (lmsg->ScrWidth() * GivenWidth->Size) / 100;
        }
      }
      /* continue... */

      case Size_Pixels:
        def_w = GivenWidth->Size;
      break;
    }
  }
  return(def_w);
}

LONG ImgClass::Height (LONG def_h, struct LayoutMessage *lmsg)
{
  if(GivenHeight)
  {
    switch(GivenHeight->Type)
    {
      case Size_Percent:
      {
        if(lmsg)
        {
          GivenHeight->Type = Size_Pixels;
          GivenHeight->Size = (lmsg->ScrHeight * GivenHeight->Size) / 100;
        }
      }
      /* continue... */

      case Size_Pixels:
        def_h = GivenHeight->Size;
      break;
    }
  }
  return(def_h);
}

BOOL ImgClass::Layout (struct LayoutMessage &lmsg)
{
  LONG width = Width(80, &lmsg);
  LONG height = Height(20, &lmsg);

  STRPTR url;
  if(!Picture && Source && (url = (STRPTR)DoMethod(lmsg.HTMLview, MUIM_HTMLview_AddPart, (ULONG)Source)))
  {
    if((Picture = lmsg.Share->ImageStorage->FindImage(url, GivenWidth ? width : 0, GivenHeight ? height : 0)))
    {
      ReceiveImage(Picture);
      width = Picture->Width;
      height = Picture->Height;
      UpdateImage(0, height, 0, 0, TRUE);

      if(Picture->Next)
        DoMethod(lmsg.HTMLview, MUIM_HTMLview_AddSingleAnim, (ULONG)Picture, (ULONG)this);
    }
    delete url;
  }
  else
  {
    if(Picture && (Picture->Flags & PicFLG_Complete) && Picture->AlphaMask)
      Flags |= FLG_Img_CreateAlpha;
  }

  width += 2*(ImgBorder+HSpace);
  height += 2*(ImgBorder+VSpace);


  if(lmsg.X + width-1 > lmsg.MaxX)
    lmsg.Newline();
  else if(width > lmsg.ScrWidth())
    lmsg.EnsureNewline();

  if(Alignment == Align_Left || Alignment == Align_Right)
  {
    lmsg.EnsureNewline();

    Top = lmsg.Y;
    Bottom = lmsg.Y + height - 1;

    struct FloadingImage *img = new struct FloadingImage(Top, Left, width, height, this, lmsg.Parent);
    Left = lmsg.AddImage(img, Alignment == Align_Right);

    lmsg.TopChange = min(lmsg.TopChange, Top);
  }
  else
  {
    Left = lmsg.X;
    lmsg.X += width;

    Top = lmsg.Y - (height-1);
    Bottom = lmsg.Y;

    LONG baseline;
    switch(Alignment)
    {
      case Align_Top:
      {
        Top = lmsg.Y;
        Bottom = lmsg.Y + (height-1);
        lmsg.SetLineHeight(height);
      }
      break;

      case Align_Middle:
      {
        baseline = height/2;
        LONG offset = (height - baseline)-1;
        Top += offset;
        Bottom += offset;
        lmsg.UpdateBaseline(height, baseline);
      }
      break;

      default:
      {
        baseline = height-1;
        lmsg.UpdateBaseline(height, baseline);
      }
      break;
    }

    struct ObjectNotify *notify = new struct ObjectNotify(Left, Baseline, this);
    lmsg.AddNotify(notify);
  }
  Flags |= FLG_WaitingForSize;

   return TRUE;
}

VOID ImgClass::AdjustPosition (LONG x, LONG y)
{
  Left += x;
  SuperClass::AdjustPosition(x, y);
}

VOID ImgClass::MinMax (struct MinMaxMessage &mmsg)
{
  struct LayoutMessage *lmsg = mmsg.LMsg;
  LONG width = Width(80, lmsg);

  STRPTR url;
  if(!Picture && Source && (url = (STRPTR)DoMethod(lmsg->HTMLview, MUIM_HTMLview_AddPart, (ULONG)Source)))
  {
    if((Picture = lmsg->Share->ImageStorage->FindImage(url, GivenWidth ? width : 0, Height(0, lmsg))))
    {
      ReceiveImage(Picture);
      width = Picture->Width;
      UpdateImage(0, Picture->Height, 0, 0, TRUE);

      if(Picture->Next)
        DoMethod(lmsg->HTMLview, MUIM_HTMLview_AddSingleAnim, (ULONG)Picture, (ULONG)this);
    }
    delete url;
  }

  width += 2*(HSpace+ImgBorder);

  if(Alignment == Align_Left)
    width += 5;

  mmsg.Min = max(width, mmsg.Min);
  mmsg.X += width;

  Flags |= FLG_KnowsMinMax;
}

VOID ImgClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  AttrClass::Parse(pmsg);

  BOOL ismap = FALSE;
  struct ArgList args[] =
  {
    { "ALT",    &AltText,     ARG_STRING,   NULL },
    { "SRC",    &Source,      ARG_URL,      NULL },
    { "NAME",   &Name,        ARG_URL,      NULL },
    { "WIDTH",  &GivenWidth,  ARG_VALUE,    NULL },
    { "HEIGHT", &GivenHeight, ARG_VALUE,    NULL },
    { "BORDER", &ImgBorder,   ARG_NUMBER,   NULL },
    { "ALIGN",  &Alignment,   ARG_KEYWORD,  ImgAlignKeywords },
    { "HSPACE", &HSpace,      ARG_NUMBER,   NULL },
    { "VSPACE", &VSpace,      ARG_NUMBER,   NULL },
    { "USEMAP", &Map,         ARG_URL,      NULL },
    { "ISMAP",  &ismap,       ARG_SWITCH,   NULL },
    { NULL,     NULL,         0,            NULL }
  };
  ImgBorder = pmsg.Linkable;
  Alignment = (ULONG)-1;
  ScanArgs(pmsg.Locked, args);
  Alignment++;
  if(Alignment == Align_Center)
    Alignment = Align_None;
  if(ismap)
    Flags |= FLG_Img_IsMap;
}

BOOL ImgClass::ReceiveImage (struct PictureFrame *pic)
{
  BOOL relayout = FALSE;
  Picture = pic;
  pic->LockPicture();
  YStart = YStop = 0;

  if(!GivenWidth)
  {
    GivenWidth = new struct ArgSize(pic->Width, Size_Pixels);
    relayout = TRUE;
  }

  if(!GivenHeight)
  {
    GivenHeight = new struct ArgSize(pic->Height, Size_Pixels);
    relayout = TRUE;
  }

  return(relayout);
}

BOOL ImgClass::UpdateImage (LONG ystart, LONG ystop, LONG top, LONG bottom, BOOL last)
{
  if(!Picture)
    return(FALSE);

  if(last && Picture->AlphaMask && CyberGfxBase)
  {
    ystart = 0;
    Flags |= FLG_Img_CreateAlpha;
  }

  BOOL redraw;
  LONG pic_top = Top+ystart;
  if((redraw = (pic_top <= bottom && pic_top+(ystop-ystart) >= top)))
  {
    YStart = ystart;
    YStop = ystop;
  }
  else
  {
    YStart = 0;
    YStop = (Picture->Flags & PicFLG_Full) ? Picture->Height : ystop;
  }

  return(redraw);
}

BOOL ImgClass::FlushImage (LONG top, LONG bottom)
{
  if(Picture)
  {
    YStop = 0;
    Picture->UnLockPicture();
    Picture = NULL;
    Flags |= FLG_Img_DrawBackground;
    return(Top <= bottom && Bottom >= top);
  }
  return(FALSE);
}

VOID ImgClass::Render (struct RenderMessage &rmsg)
{
  struct RastPort *rp = rmsg.RPort;
  LONG x1 = Left+HSpace-rmsg.OffsetX, y1 = Top+VSpace-rmsg.OffsetY;
  LONG width = Width(), height = Height();

  if(!width || !height)
    return;

  if(ImgBorder)
  {
    LONG x2 = x1+width+(2*ImgBorder)-1, y2 = y1+height+(2*ImgBorder)-1;

    ULONG border = ImgBorder-1;
    SetAPen(rp, rmsg.Colours[(rmsg.Textstyles & TSF_ALink) ? Col_ALink : ((rmsg.Textstyles & TSF_Link) ? ((rmsg.Textstyles & TSF_VLink) ? Col_VLink : Col_Link) : Col_Text)]);

    RectFill(rp, x1+ImgBorder, y1, x2, y1+border);
    RectFill(rp, x2-border, y1+ImgBorder, x2, y2);
    RectFill(rp, x1, y2-border, x2-ImgBorder, y2);
    RectFill(rp, x1, y1, x1+border, y2-ImgBorder);

    x1 += ImgBorder;
    y1 += ImgBorder;
  }

  if(rmsg.RedrawLink)
    return;

  LONG x2 = x1+width-1, y2 = y1+height-1;
  if(Picture)
  {
    LONG pass_height = YStop-YStart;
    if(Picture->Mask)
    {
      struct RastPort *tmprp;
      if(rmsg.TargetObj == (class SuperClass *)this && (tmprp = rmsg.ObtainDoubleBuffer(width, pass_height)))
      {
        LONG  xoffset = rmsg.Left + x1 - rmsg.MinX,
            yoffset = rmsg.Top  + y1 - rmsg.MinY;

        rmsg.RPort = tmprp;
        rmsg.BackgroundObj->DrawBackground(rmsg, 0, 0, width-1, pass_height-1, xoffset, yoffset+YStart);
        BltMaskRPort(Picture->BMp, 0, YStart, tmprp, 0, 0, width, pass_height, Picture->Mask);
        BltBitMapRastPort(tmprp->BitMap, 0, 0, rp, x1, y1+YStart, width, pass_height, 0x0c0);
        rmsg.RPort = rp;
      }
      else
      {
        BltMaskRPort(Picture->BMp, 0, YStart, rp, x1, y1+YStart, width, pass_height, Picture->Mask);
      }
    }
    else
    {
      if(Flags & FLG_Img_CreateAlpha && rmsg.BackgroundObj->ReadyForAlpha())
      {
        struct RastPort *tmprp, srcrport, dstrport;

        if(BlendBitMap)
        {
          WaitBlit();
          FreeBitMap(BlendBitMap);
        }
        BlendBitMap = AllocBitMap(width, height, GetBitMapAttr(Picture->BMp, BMA_DEPTH), BMF_MINPLANES, Picture->BMp);

        if(BlendBitMap && (tmprp = rmsg.ObtainDoubleBuffer(width, pass_height)))
        {
          LONG  xoffset = rmsg.Left + x1 - rmsg.MinX,
              yoffset = rmsg.Top  + y1 - rmsg.MinY;

          rmsg.RPort = tmprp;

          struct RGBPixel *mixline;
          mixline = new struct RGBPixel [2*width];

          InitRastPort(&srcrport);
          srcrport.BitMap = Picture->BMp;
          InitRastPort(&dstrport);
          dstrport.BitMap = BlendBitMap;

          UBYTE *alpha = Picture->AlphaMask;
          for(UWORD y = 0; y < height; y++)
          {
            rmsg.BackgroundObj->DrawBackground(rmsg, 0, 0, width-1, 0, xoffset, yoffset+y);
            ReadPixelArray(mixline, 0, 0, width*sizeof(RGBPixel), &srcrport, 0, y, width, 1, PIXEL_FORMAT);
            ReadPixelArray(mixline, 0, 1, width*sizeof(RGBPixel), tmprp, 0, 0, width, 1, PIXEL_FORMAT);

            for(UWORD x = 0; x < width; x++)
            {
              UBYTE factor = *alpha++;
              mixline[x].SetRGB(mixline[x].Scale(factor) + mixline[x+width].Scale(255-factor));
            }

            WritePixelArray(mixline, 0, 0, width*sizeof(RGBPixel), &dstrport, 0, y, width, 1, PIXEL_FORMAT);
          }

          delete mixline;
          rmsg.RPort = rp;
        }

        Flags &= ~FLG_Img_CreateAlpha;
      }

      struct BitMap *bmp = BlendBitMap ? BlendBitMap : Picture->BMp;
      BltBitMapRastPort(bmp, 0, YStart, rp, x1, y1+YStart, width, pass_height, 0x0c0);
    }

    YStart = 0;
    if(Picture->Flags & PicFLG_Full)
      YStop = height;
    y1 += YStop;
  }
  else if(Flags & FLG_Img_DrawBackground)
  {
    LONG  xoffset = rmsg.Left + x1 - rmsg.MinX,
        yoffset = rmsg.Top  + y1 - rmsg.MinY;

    rmsg.BackgroundObj->DrawBackground(rmsg, x1, y1, x1+width-1, y1+height-1, xoffset, yoffset);
    Flags &= ~FLG_Img_DrawBackground;
  }

  LONG col1, col2;
  if(rmsg.Textstyles & TSF_Link)
  {
    col1 = rmsg.Colours[Col_Halfshadow];
    col2 = rmsg.Colours[Col_Halfshine];
  }
  else
  {
    col1 = rmsg.Colours[Col_Halfshine];
    col2 = rmsg.Colours[Col_Halfshadow];
  }

  if(width > 1 && y2 >= y1)
  {
    SetAPen(rp, col1);
    RectFill(rp, x1, y2, x2, y2);
    RectFill(rp, x2, y1, x2, y2);
    SetAPen(rp, col2);
    if(YStop == 0)
      RectFill(rp, x1, y1, x2, y1);
    RectFill(rp, x1, y1, x1, y2);
  }

  if(!Picture)
  {
    STRPTR alt = AltText ? AltText : (STRPTR)"[Image]";
    ULONG length = strlen(alt);
//    for(UWORD i = AltText ? Font_H1 : Font_H6; i <= Font_H6; i++)
    for(UWORD i = Font_H5; i <= Font_H6; i++)
    {
      UWORD pixels = MyTextLength(rmsg.Share->Fonts[i], alt, length);

      if((pixels+2) <= width-4 && (rmsg.Share->Fonts[i]->tf_YSize+2) <= height-4)
      {
        LONG startx = x1 + (width-(pixels+2))/2;
        LONG starty = y1 + ((height-(rmsg.Share->Fonts[i]->tf_YSize+2))/2);
        SetAPen(rp, rmsg.Colours[Col_Text]);
        SetFont(rp, rmsg.Share->Fonts[i]);
        SetSoftStyle(rp, 0L, TSF_StyleMask);
        Move(rp, startx, starty+rp->TxBaseline);
        Text(rp, alt, length);

        break;
      }
    }
  }
}

BOOL ImgClass::TestPixel(UBYTE *line, LONG x)
{
  return(line[x >> 3] & (1 << (x%7)));
}

