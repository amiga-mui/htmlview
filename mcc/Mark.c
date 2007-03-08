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

#include <string.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <devices/clipboard.h>
#include <libraries/mui.h>

#include "General.h"
#include "Classes.h"
#include "Mark.h"

#include "classes/TextClass.h"

#include "private.h"

BOOL MarkMessage::OpenTheClipboard ()
{
	if(ClipPort = CreateMsgPort())
	{
		if(ClipReq = (struct IOClipReq *)CreateIORequest(ClipPort, sizeof(IOClipReq)))
		{
			if(!OpenDevice("clipboard.device", 0, IOReq, 0))
			{
				ClipBuffer = new UBYTE [CLIPBUFFERSIZE];

				ClipReq->io_ClipID	= 0;
				ClipReq->io_Offset	= 20;
				ClipReq->io_Command	= CMD_WRITE;
				return(TRUE);
			}
			DeleteIORequest(IOReq);
		}
		DeleteMsgPort(ClipPort);
	}
	return(FALSE);
}

VOID MarkMessage::CloseTheClipboard ()
{
	FlushClipBuffer();
	delete ClipBuffer;

	ULONG len = ClipReq->io_Offset - 20;
	ULONG IFF_Head[] = { 'FORM', (len + 13) & ~1, 'FTXT', 'CHRS', len };

	ClipReq->io_Offset	= 0;
	ClipReq->io_Data		= (STRPTR)IFF_Head;
	ClipReq->io_Length	= sizeof(IFF_Head);
	DoIO(IOReq);

	ClipReq->io_Command	= CMD_UPDATE;
	DoIO(IOReq);

	CloseDevice(IOReq);
	DeleteIORequest(IOReq);
	DeleteMsgPort(ClipPort);
}

VOID MarkMessage::WriteLF ()
{
	WriteText("\n", 1);
}

VOID MarkMessage::WriteText (STRPTR text, ULONG len)
{
	if(Copy)
	{
		if(ClipLength + len > CLIPBUFFERSIZE)
			FlushClipBuffer();

		if(len < CLIPBUFFERSIZE)
		{
			memcpy(ClipBuffer+ClipLength, text, len);
			ClipLength += len;
		}
		else
		{
			ClipReq->io_Data		= text;
			ClipReq->io_Length	= len;
			DoIO(IOReq);
		}
	}
}

VOID MarkMessage::FlushClipBuffer ()
{
	ClipReq->io_Data		= (char*)ClipBuffer;
	ClipReq->io_Length	= ClipLength;
	DoIO(IOReq);
	ClipLength = 0;
}

VOID MarkMessage::AddBox (class TextClass *child, LONG b, LONG e)
{
	struct BoxItem *node = new struct BoxItem(child, b, e);
	LastBox->Next = node;
	LastBox = node;
}

VOID MarkMessage::DrainBoxList (struct RastPort *rp, LONG xoffset, LONG yoffset)
{
	struct BoxItem *prev, *first = FirstBox;
	SetDrMd(rp, COMPLEMENT);
	while(first)
	{
		first->Obj->RenderMarked(rp, first->MarkBegin, first->MarkEnd, xoffset, yoffset);
		prev = first;
		first = first->Next;
		delete prev;
	}
	SetDrMd(rp, JAM1);

	FirstBox = NULL;
	LastBox = (struct BoxItem *)&FirstBox;
}

#define BETWEEN(min, value, max) (value >= min && value <= max)

VOID TruncArgs::TableTruncCoords (LONG &x, LONG &y, LONG flag)
{
	if(y < Top || (y < Bottom && x < Left))
	{
		x = Left;
		y = Top;
		*Flags |= flag;
	}
	else if(y > Bottom || (y > Top && x > Right))
	{
		x = Right;
		y = Bottom;
	}
}

VOID TruncArgs::TablePreRightTruncCoords (LONG &x, LONG &y)
{
	if(!BETWEEN(Left, x, Right))
		y = y < Bottom ? Top : Bottom;
}

VOID TruncArgs::TablePostRightTruncCoords (LONG &x, LONG &y)
{
	if(BETWEEN(Left, x, Right) && BETWEEN(Top, y, Bottom))
		y = Bottom;
}

VOID TruncArgs::TableLeftTruncCoords (LONG &x, LONG &y, LONG flag)
{
	if(BETWEEN(Left, x, Right) && BETWEEN(Top, y, Bottom))
	{
		*Flags |= flag;
		y = Top;
	}
}

VOID TruncArgs::TDTruncCoords (LONG &x, LONG &y, LONG flag)
{
	if(!(BETWEEN(Top, y, Bottom) && BETWEEN(Left, x, Right)))
	{
		if(*Flags & flag)
		{
			x = Left;
			y = Top;
		}
		else
		{
			x = Right+1;	/* '+1' ensures that last character will be included */
			y = Bottom;
		}
	}
}

BOOL TruncArgs::Inside (LONG x, LONG y)
{
	return(BETWEEN(Left, x, Right) && BETWEEN(Top, y, Bottom));
}
