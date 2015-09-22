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

#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <dos/dos.h>
#include <proto/exec.h>

/* Protos */
VOID DecodeImage (Object *obj, struct IClass *cl, struct ImageList *image, struct HTMLviewData *data);

struct ImageCacheItem
{
  ImageCacheItem (STRPTR url, struct PictureFrame *pic);
  ~ImageCacheItem ();

  struct ImageCacheItem *Next;
  STRPTR URL;
  struct PictureFrame *Picture;
};

class ImageCache
{
  public:
    ImageCache (ULONG maxsize);
    ~ImageCache ();
    VOID AddImage (STRPTR url, struct PictureFrame *pic);
    struct PictureFrame *FindImage (STRPTR url, ULONG width, ULONG height);
    VOID FlushCache (STRPTR url = NULL);

    struct ImageCacheItem *FirstEntry, *LastEntry;
    ULONG CurrentSize, MaxSize;
};

enum
{
  StatusPending = 0,
  StatusDecoding,
  StatusError,
  StatusAborted,
  StatusDone
};


struct DecodeItem
{
  DecodeItem (Object *obj, struct HTMLviewData *data, struct ImageList *image);

  struct DecodeItem *Prev, *Next;
  class DecodeQueueManager *DecodeQueue;

  UWORD Status, _pad;
  BOOL Abort, Started;
  struct PictureFrame *Picture;
  struct AnimInfo *Anim;
  LONG CurrentY, LastY;
  ULONG CurrentPass, LastPass;

  Object *Obj;
  ULONG PageID;
  struct ImageList *Images;
  struct HTMLviewData *Data;
  struct Task *Thread;

  VOID Enter () { ObtainSemaphore(&Mutex); }
  VOID Leave () { ReleaseSemaphore(&Mutex); }
  struct SignalSemaphore Mutex;

  VOID Start (struct PictureFrame *pic);
  BOOL Update ();
};

class DecodeQueueManager
{
  public:
    DecodeQueueManager ()
    {
      InitSemaphore(&Mutex);
    }

    ~DecodeQueueManager ();
    VOID InsertElm (struct DecodeItem *item);
    VOID RemoveElm (struct DecodeItem *item);
    ULONG DumpQueue ();
    VOID InvalidateQueue (Object *obj);

    BOOL IsEmpty ()
    {
      return(Queue ? FALSE : TRUE);
    }

  protected:
    struct DecodeItem *Queue;
    struct SignalSemaphore Mutex;
};

enum
{
  IDA_HTMLviewTags = TAG_USER | 0x1000,

  IDA_StatusStruct,   /* struct DecodeQueue * */
  IDA_HTMLview,     /* Object * */
  IDA_LoadHook,     /* struct Hook * */
  IDA_LoadMsg,      /* struct HTMLview_LoadMsg * */
  IDA_StartBuffer,    /* UBYTE [10] */
  IDA_BytesInBuffer,  /* ULONG */

  IDA_NumberOf
};

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack(2)
  #elif defined(__VBCC__)
    #pragma amiga-align
  #endif
#endif

struct DecoderData
{
  Object *HTMLview;
  struct Hook *LoadHook;
  struct HTMLview_LoadMsg *LoadMsg;
  struct DecodeItem *StatusItem;
  struct Screen *Scr;
  class ScaleEngine *ImgObj;
  UBYTE *StartBuffer;
  ULONG BytesInBuffer, Gamma;
  BOOL NoTransparency;
};

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack()
  #elif defined(__VBCC__)
    #pragma default-align
  #endif
#endif

#endif
