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

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/datatypes.h>
#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <datatypes/pictureclassext.h>
#include <mui/ipc_mcc.h>

#include "General.h"
#include "HTMLviewDispatcher.h"

ImageCacheEntry::ImageCacheEntry (STRPTR url, Object *dt, ULONG flags, STRPTR filename)
{
	URL = new UBYTE [strlen(url)+1];
	strcpy(URL, url);

	Filename = new UBYTE [strlen(filename)+1];
	strcpy(Filename, filename);

	DTObject = dt;
	Flags = flags;
}

ImageCacheEntry::~ImageCacheEntry ()
{
	DisposeDTObject(DTObject);
/*	if(Flags & MUIF_HTMLview_LoadImages_DeleteAfterUse)
		DeleteFile(Filename+7);
*/
	delete URL;
	delete Filename;
}

VOID DisposeImages (struct ImageCacheEntry *current)
{
	struct ImageCacheEntry *prev;
	while(current)
	{
		prev = current;
		current = current->Next;
		delete prev;
	}
}

VOID ImageCache::AddImage (STRPTR url, Object *dt, ULONG flags, STRPTR filename)
{
	struct ImageCacheEntry *entry;
	entry = new struct ImageCacheEntry(url, dt, flags, filename);
	if(entry->Next = FirstEntry)
		FirstEntry->Prev = entry;
	FirstEntry = entry;
	if(ImageCount++ > 40)
	{
		entry = FirstEntry;
		for(UWORD i = 0; i < 35; i++)
			entry = entry->Next;
		entry->Prev->Next = NULL;
		DisposeImages(entry);
		ImageCount = 35;
	}
}

Object *ImageCache::FindImage (STRPTR url)
{
	Object *result = NULL;

	struct ImageCacheEntry *current = FirstEntry;
	while(current)
	{
		if(!strcmp(url, current->URL))
		{
			result = current->DTObject;

			if(FirstEntry != current)
			{
				if(current->Prev->Next = current->Next)
					current->Next->Prev = current->Prev;

				current->Next = FirstEntry;
				current->Prev = NULL;

				FirstEntry->Prev = current;
				FirstEntry = current;
			}

			break;
		}
		current = current->Next;
	}
	return(result);
}

VOID ImageCache::FlushCache ()
{
	DisposeImages(FirstEntry);
	FirstEntry = NULL;
	ImageCount = 0;
}

struct IPCMsg
{
	IPCMsg (ULONG handle, ULONG flags = 0)
	{
		ipc_msg.ipc_Header.mn_Length = sizeof(struct IPCMsg);
		ipc_msg.ipc_Flags = flags;
		ipc_msg.ipc_Handle = handle;
	}

	ULONG MethodID;
	struct IPC_Message ipc_msg;
	ULONG PageID;
	struct ImageList *Images;
	Object *DTObject;
	struct Task *ImageDecoderThread;
};


VOID ImageDecoderThread ()
{
	LONG args[6];
	struct RDArgs *readargs;
	if(readargs = ReadArgs("/A/N,/A/N,/A/N,/A/N,/A/N,/A/F", args, NULL))
	{
		ULONG pageID = *((ULONG *)args[0]);
		struct ImageList *images = *((struct ImageList **)args[1]);
		Object *ipc = *((Object **)args[2]);
		ULONG handle = *((ULONG *)args[3]);
		struct Screen *scr = *((struct Screen **)args[4]);
		STRPTR url = (STRPTR)args[5];

		Object *dt = NULL;
		if(dt = NewDTObject(url,
			DTA_GroupID,				GID_PICTURE,
			PDTA_DestMode,				MODE_V43,
			PDTA_Remap,					TRUE,
			PDTA_FreeSourceBitMap,	TRUE,
			PDTA_Screen,				scr,
			PDTA_UseFriendBitMap,	TRUE,
			TAG_DONE))
		{
			DoMethod(dt, DTM_PROCLAYOUT);
		}

		struct IPCMsg ipcmsg(handle, IPCFlg_Unlock);
		ipcmsg.MethodID				= MUIM_IPC_DoMessage;
		ipcmsg.PageID					= pageID;
		ipcmsg.Images					= images;
		ipcmsg.DTObject				= dt;
		ipcmsg.ImageDecoderThread	= FindTask(NULL);

		if(!DoMethodA(ipc, (Msg)&ipcmsg))
			DisposeDTObject(dt);

		UnlockPubScreen(NULL, scr);
	}
}
