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

#include <stdio.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "IM_ColourManager.h"

struct SignalSemaphore Mutex;
class ColourManager *FirstCMap = NULL;

extern "C"
{
	VOID _INIT_5_CMapMutex ()
	{
		memset(&Mutex, 0, sizeof(Mutex));
		InitSemaphore(&Mutex);
	}
}

class ColourManager *ObtainCMap (struct Screen *scr)
{
	ObtainSemaphore(&Mutex);

	struct ColorMap *skey = scr->ViewPort.ColorMap;
	struct ColourManager *last = NULL, *first = FirstCMap;
	while(first)
	{
		if(first->ColourMap == skey)
			break;

		last = first;
		first = first->Next;
	}

	if(!first)
	{
		if(!(first = new ColourManager(scr)))
			goto error;

		if(last)
		{
			last->Next = first;
			first->Prev = last;
		}
		else
		{
			FirstCMap = first;
		}
	}
	first->LockCnt++;

error:
	ReleaseSemaphore(&Mutex);
	return(first);
}

VOID ReleaseCMap (class ColourManager *cmap)
{
	if(!cmap)
		return;

	ObtainSemaphore(&Mutex);

	if(--cmap->LockCnt == 0)
	{
		if(cmap == FirstCMap)
			FirstCMap = cmap->Next;
		if(cmap->Prev)
			cmap->Prev->Next = cmap->Next;
		if(cmap->Next)
			cmap->Next->Prev = cmap->Prev;

		delete cmap;
	}

	ReleaseSemaphore(&Mutex);
}

ColourManager::ColourManager (struct Screen *scr)
{
	ColourMap = scr->ViewPort.ColorMap;

	{
		struct RGBPixel colours[256];
		ULONG i = 0;
		for(WORD b = 3; b >= 0; b--)
		{
			for(WORD g = 7; g >= 0; g--)
			{
				for(WORD r = 7; r >= 0; r--)
				{
					colours[i].R = (r << 5)+(31*r/7);
					colours[i].G = (g << 5)+(31*g/7);
					colours[i].B = (b << 6)+(63*b/3);
					i++;
				}
			}
		}

		for(UWORD outer = 0; outer < 32; outer++)
		{
			for(i = outer; i < 256; i += 32)
			{
				LONG r = colours[i].R, g = colours[i].G, b = colours[i].B;
				LONG pen = ObtainBestPen(ColourMap, PRECISION(r), PRECISION(g), PRECISION(b), OBP_Precision, PRECISION_IMAGE, OBP_FailIfBad, FALSE, TAG_DONE);
				Allocated[colours[i].Index()] = pen;

				ULONG table[3];
				GetRGB32(ColourMap, pen, 1, table);
				CValues[colours[i].Index()].Init(table);
			}
		}
	}

/*	for(WORD b = 3; b >= 0; b--)
	{
		for(WORD g = 7; g >= 0; g--)
		{
			for(WORD r = 7; r >= 0; r--)
			{
				LONG pen = ObtainBestPen(ColourMap, PRECISION((r << 5)+(31*r/7)), PRECISION((g << 5)+(31*g/7)), PRECISION((b << 6)+(63*b/3)), OBP_Precision, PRECISION_IMAGE, OBP_FailIfBad, FALSE, TAG_DONE);
				UWORD i = r << 5 | g << 2 | b;
				Allocated[i] = pen;

				ULONG table[3];
				GetRGB32(ColourMap, pen, 1, table);
				CValues[i].Init(table);
			}
		}
	}*/
}

ColourManager::~ColourManager ()
{
	for(UWORD i = 0; i < 256; i++)
		ReleasePen(ColourMap, Allocated[i]);
}
