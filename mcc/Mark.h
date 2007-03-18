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

#ifndef MARK_H
#define MARK_H

#define MarkFLG_FirstCoordTaken		(1 << 0)
#define MarkFLG_SecondCoordTaken		(1 << 1)
#define MarkFLG_LastCoordTaken		(1 << 2)

#define CLIPBUFFERSIZE (1024/2)

struct BoxItem
{
	BoxItem (class TextClass *obj, LONG b, LONG e)
	{
		MarkBegin = b;
		MarkEnd = e;
		Obj = obj;
	}

	struct BoxItem *Next;
	LONG MarkBegin, MarkEnd;
	class TextClass *Obj;
};

struct MarkMessage
{
	MarkMessage ()
	{
		FirstBox = NULL;
		LastBox = (struct BoxItem *)&FirstBox;
	}

	VOID Reset (LONG x, LONG y)
	{
		LastX = X1 = X2 = x;
		LastY = Y1 = Y2 = y;
	}

	BOOL Enabled ()
	{
		UWORD flgs;

		if((flgs = (Flags & (MarkFLG_FirstCoordTaken | MarkFLG_SecondCoordTaken))))
  		return(flgs != (MarkFLG_FirstCoordTaken | MarkFLG_SecondCoordTaken));
		else
      return(FALSE);
	}

	VOID AddBox (class TextClass *child, LONG b, LONG e);
	VOID DrainBoxList (struct RastPort *rp, LONG xoffset, LONG yoffset);

	VOID WriteText(CONST_STRPTR text, ULONG len);
	VOID WriteLF ();
	VOID FlushClipBuffer ();

	/* Keep this as a group! */
	LONG X1, Y1, X2, Y2;
	LONG LastX, LastY;
	UWORD Flags, PAD;
	LONG TableSpace;
	/* End of group */

	BOOL Newline;		/* We're at the beginning of a line */
	BOOL Copy;			/* We should export our contents */
	UWORD OL_Type;

	BOOL OpenTheClipboard ();
	VOID CloseTheClipboard ();

	union
	{
		struct IORequest *IOReq;
		struct IOClipReq *ClipReq;
	};
	struct MsgPort *ClipPort;

	struct BoxItem *FirstBox;
	struct BoxItem *LastBox;

	UBYTE *ClipBuffer;
	ULONG ClipLength;
};

struct TruncArgs
{
	TruncArgs (LONG left, LONG top, LONG right, LONG bottom, UWORD &flags)
	{
		Left = left;
		Top = top;
		Right = right;
		Bottom = bottom;
		Flags = &flags;
	}

	VOID TableTruncCoords (LONG &x, LONG &y, LONG flag);
	VOID TablePreRightTruncCoords (LONG &x, LONG &y);
	VOID TablePostRightTruncCoords (LONG &x, LONG &y);
	VOID TableLeftTruncCoords (LONG &x, LONG &y, LONG flag);
	VOID TDTruncCoords (LONG &x, LONG &y, LONG flag);
	BOOL Inside (LONG x, LONG y);

	LONG Left, Top, Right, Bottom;
	UWORD *Flags;
};

#endif
