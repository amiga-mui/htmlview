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

#ifndef ANIMATION_H
#define ANIMATION_H

#include <dos/dos.h>

struct AnimInfo
{
	AnimInfo (Object *obj, struct HTMLviewData *data, struct PictureFrame *pic, struct ObjectList *receivers, struct AnimInfo *next);
	~AnimInfo ();
	ULONG Tick ();
	ULONG Update (ULONG diff);
	VOID Stop (Object *obj, struct AnimInfo *prev);

	struct AnimInfo *Next;
	Object *Obj;
	struct HTMLviewData *Data;
	ULONG TimeLeft;
	struct PictureFrame *FirstFrame, *CurrentFrame;
	struct ObjectList *Receivers;
	UBYTE *Mask;
	struct BitMap *BMp, MaskBMp;
	struct RastPort BMpRP, MaskRP;
	struct DateStamp LastTime;
	ULONG Flags;

	struct DecodeItem *Decode;
};

#define AnimFLG_DeleteObjList (1 << 0)

#endif
