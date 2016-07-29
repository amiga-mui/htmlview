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

#include <clib/macros.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <graphics/gfxmacros.h>

#include "General.h"

BOOL MUIG_ScrollDamage(REG(a0) struct RastPort *rp,REG(d0) WORD dx,REG(d1) WORD dy,REG(d2) WORD left,REG(d3) WORD top,REG(d4) WORD right,REG(d5) WORD bottom)
{
	#ifdef AMIGAOS
	struct Layer *layer = rp->Layer;

	BOOL scroll = TRUE;

	D(DBF_LAYER,bug("\n"));

	LockLayer(0,layer);

/*	D(DBF_LAYER,bug("scrollarea: %3ld %3ld %3ld %3ld dx=%ld dy=%ld\n",left,top,right,bottom,dx,dy));
	D(DBF_LAYER,bug("layerbound: %3ld %3ld %3ld %3ld\n",layer->bounds.MinX,layer->bounds.MinY,layer->bounds.MaxX,layer->bounds.MaxY));
	D(DBF_LAYER,bug("1stcliprec: %3ld %3ld %3ld %3ld\n",layer->ClipRect->bounds.MinX,layer->ClipRect->bounds.MinY,layer->ClipRect->bounds.MaxX,layer->ClipRect->bounds.MaxY));
*/
	if (layer->DamageList && layer->DamageList->RegionRectangle)
	{
//		D(DBF_LAYER,bug("layer damaged\n"));
	}
	else
	{
		struct Rectangle rect;
		struct ClipRect *cr;

		#ifdef MYDEBUG
		int i;
		#endif

		rect.MinX = layer->bounds.MinX + left;
		rect.MinY = layer->bounds.MinY + top;
		rect.MaxX = layer->bounds.MinX + right;
		rect.MaxY = layer->bounds.MinY + bottom;

		#ifdef MYDEBUG
		for (i=0,cr=layer->ClipRect;cr;cr=cr->Next,i++)
		#else
		for (cr=layer->ClipRect;cr;cr=cr->Next)
		#endif
		{
			if (rect.MinX < cr->bounds.MinX) continue;
			if (rect.MaxX > cr->bounds.MaxX) continue;
			if (rect.MinY < cr->bounds.MinY) continue;
			if (rect.MaxY > cr->bounds.MaxY) continue;
//			D(DBF_LAYER,bug("scrollregion in cliprect #%ld\n",i));
			scroll = FALSE;
			break;
		}
	}

	UnlockLayer(layer);

	#ifdef MYDEBUG
	if (!scroll && ForceScroll)
	{
//		D(DBF_LAYER,bug("ScrollRaster forced by env:muiscroll\n"));
		scroll = TRUE;
	}
	#endif

	if (scroll)
	{
		UBYTE oldmask = rp->Mask;
		SetWrMsk(rp,0);
//		D(DBF_LAYER,bug("ScrollRaster0(%08lx,%ld,%ld,%ld,%ld,%ld,%ld)\n",rp,dx,dy,left,top,right,bottom));
		ScrollRaster(rp,dx,dy,left,top,right,bottom);
		SetWrMsk(rp,oldmask);
	}
	else
	{
//		D(DBF_LAYER,bug("skip ScrollRaster0(%08lx,%ld,%ld,%ld,%ld,%ld,%ld)\n",rp,dx,dy,left,top,right,bottom));
	}

	return(scroll);
	#endif /* AMIGAOS */

	#ifdef POS
	{
		UBYTE oldmask = rp->Mask;
		SetWrMsk(rp,0);
//		D(DBF_LAYER,bug("ScrollRaster0(%08lx,%ld,%ld,%ld,%ld,%ld,%ld)\n",rp,dx,dy,left,top,right,bottom));
		ScrollRaster(rp,dx,dy,left,top,right,bottom);
		SetWrMsk(rp,oldmask);
		return(TRUE);
	}
	#endif /* POS */
}



LONG MUIG_ScrollRaster(struct RastPort *rp, WORD dx, WORD dy, WORD left, WORD top, WORD right, WORD bottom)
{
	WORD width  = right - left + 1;
	WORD height = bottom - top + 1;

	if (width <=0) return(-1);
	if (height<=0) return(-2);

	if (dx || dy)
	{
		WORD adx = ABS(dx);
		WORD ady = ABS(dy);

		if (adx<width && ady<height)
		{
			WORD cw = width  - adx;
			WORD ch = height - ady;
			WORD x1 = left;
			WORD x2 = left;
			WORD y1 = top;
			WORD y2 = top;

			if (dx>=0) x1 += dx;
			else       x2 -= dx;

			if (dy>=0) y1 += dy;
			else       y2 -= dy;

			ClipBlit(rp,x1,y1,rp,x2,y2,cw,ch,0xc0);
		}

		if (rp->Layer && rp->Layer->Window && (((struct Window *)rp->Layer->Window)->Flags & WFLG_SIMPLE_REFRESH))
			MUIG_ScrollDamage(rp,dx,dy,left,top,right,bottom);

		return(1);
	}

	return(0);
}



