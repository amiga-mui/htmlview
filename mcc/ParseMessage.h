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

#ifndef PARSEMESSAGE_H
#define PARSEMESSAGE_H

#include <dos/dos.h>
#include <exec/semaphores.h>
#include <intuition/classusr.h>

#include "TagInfo.h"
#include "HTMLview_mcc.h"

struct ParseMessage
{
	ParseMessage (STRPTR buf, ULONG size, struct Hook *loadhook, ULONG pageid);
	virtual ~ParseMessage ();
	VOID SkipSpaces ();
	VOID NextStartBracket ();
	VOID NextEndBracket ();
	VOID AdvancedNextEndBracket ();
	virtual BOOL Fetch (UWORD len = 1);
	VOID SetLock () { Locked = Current; }
	VOID SetError (ULONG);
	BOOL OpenURL (STRPTR url, Object *htmlview, ULONG flags);
	LONG ReadURL (STRPTR buffer, ULONG size);
	VOID WriteURL (STRPTR buffer);
	VOID CloseURL ();

	struct Hook *LoadHook;
	struct HTMLview_LoadMsg LoadMsg;
	Object *HTMLview;
	class HostClass *Host;

	STRPTR Buffer;
	STRPTR Locked;
	STRPTR Current;
	STRPTR Upper;
	ULONG Size;
	ULONG Total;
//	ULONG Length;
	ULONG Parsed;
	UWORD Status;
	ULONG PageID;

	BOOL Linkable;

	BOOL PendingSpace, PendingNewline, Gadgets;
	UWORD CellAlignment, CellVAlignment;

	/* We're in a <NoFrames>-tag, ignore meta-tags etc. */
	BOOL NoFrames, _pad;

	/* Set when found */
	STRPTR Title;

	/* Frame inherits from Frameset (albeit not according to the standard) */
	ULONG FrameBorder;

	/* OL - enumerated lists */
	ULONG OL_Cnt[3];
	LONG OL_Level;

	UBYTE OpenCounts[tag_NumberOf];
	UBYTE OpenGroups[group_NumberOf];
};

struct BufferParseMessage : public ParseMessage
{
	BufferParseMessage (STRPTR buf, ULONG size);
//	~BufferParseMessage () { Buffer = NULL; }
	BOOL Fetch (UWORD len = 1);
};

#define ParseMsg_OutOfMem			(1 << 0)
#define ParseMsg_Succes				(1 << 1)
#define ParseMsg_UnexpectedEnd	(1 << 2)
#define ParseMsg_Aborted			(1 << 3)

#endif
