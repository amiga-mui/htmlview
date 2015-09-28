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

#include <string.h>
#include <dos/dostags.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <clib/macros.h>

#if defined(__amigaos4__)
#include <exec/emulation.h>
#elif defined(__MORPHOS__)
#include <emul/emulinterface.h>
#endif

#include "ImageManager.h"
#include "IM_Output.h"
#include "ImageDecoder.h"
#include "Animation.h"
#include "SharedData.h"

#include "classes/HostClass.h"
#include "classes/ImgClass.h"
#include "classes/SuperClass.h"

//#include "private.h"

#include "SDI_stdarg.h"
#include <stdio.h>
#include <new>

#ifdef __amigaos4__
#define GetImageDecoderClass(base) ( (struct IClass *)(IExec->EmulateTags)(base, ET_Offset, -30, ET_RegisterA6, base, ET_SaveRegs, TRUE, TAG_DONE) )
#elif defined(__MORPHOS__)
#define GetImageDecoderClass(base) (struct IClass *)({ REG_A6 = (ULONG) (base); MyEmulHandle->EmulCallDirectOS(-30); })
#else
#define GetImageDecoderClass(base) ( (struct IClass *(*)(REG(a6, struct Library *))) ((UBYTE *)base-30) )(base)
#endif

ImageCacheItem::ImageCacheItem (STRPTR url, struct PictureFrame *pic)
{
  URL = new (std::nothrow) char[strlen(url)+1];
  if (URL)
  {
	  strcpy(URL, url);
	  Picture = pic;
	  pic->LockPicture();
  }
}

ImageCacheItem::~ImageCacheItem ()
{
  Picture->UnLockPicture();
  delete URL;
}

ImageCache::ImageCache (ULONG maxsize)
{
  MaxSize = maxsize;
  LastEntry = (struct ImageCacheItem *)&FirstEntry;
}

ImageCache::~ImageCache ()
{
  FlushCache();
}

VOID ImageCache::AddImage (STRPTR url, struct PictureFrame *pic)
{
  struct ImageCacheItem *item = new (std::nothrow) struct ImageCacheItem (url, pic);
  if (item)
  {
    LastEntry = (LastEntry->Next = item);
    CurrentSize += pic->Size();

    struct ImageCacheItem *preprev = NULL, *prev, *first = FirstEntry;
    while(CurrentSize > MaxSize && first)
    {
      prev = first;
      first = first->Next;

      if(prev->Picture->LockCnt == 1)
      {
        if(prev == FirstEntry)
          if(!(FirstEntry = first))
            LastEntry = (struct ImageCacheItem *)&FirstEntry;

        CurrentSize -= prev->Picture->Size();
        delete prev;

        if(preprev)
          preprev->Next = first;
      }
      else
      {
        preprev = prev;
      }
    }
    if(preprev && !preprev->Next)
      LastEntry = preprev;
  }
}

struct PictureFrame *ImageCache::FindImage (STRPTR url, ULONG width, ULONG height)
{
  struct ImageCacheItem *prev, *first = FirstEntry;
  while(first)
  {
    if(!strcmp(first->URL, url) && first->Picture->MatchSize(width, height))
    {
      if(first != LastEntry)
      {
        if(first == FirstEntry)
            FirstEntry = first->Next;
        else  prev->Next = first->Next;
        LastEntry = (LastEntry->Next = first);
        first->Next = NULL;
      }
      return(first->Picture);
    }
    prev = first;
    first = first->Next;
  }
  return(NULL);
}

inline BOOL Match (STRPTR url, struct ImageCacheItem *item)
{
  BOOL res;
  switch((LONG)url)
  {
    case MUIV_HTMLview_FlushImage_All:
      res = TRUE;
    break;

    case MUIV_HTMLview_FlushImage_Displayed:
      res = item->Picture->LockCnt > 1;
    break;

    case MUIV_HTMLview_FlushImage_Nondisplayed:
      res = item->Picture->LockCnt == 1;
    break;

    default:
      res = !strcmp(url, item->URL);
    break;
  }
  return(res);
}

VOID ImageCache::FlushCache (STRPTR url)
{
  struct ImageCacheItem *prev, *preprev = NULL, *first = FirstEntry;
  while(first)
  {
    prev = first;
    first = first->Next;

    if(Match(url, prev))
    {
      if(prev == FirstEntry)
        if(!(FirstEntry = first))
          LastEntry = (struct ImageCacheItem *)&FirstEntry;

      CurrentSize -= prev->Picture->Size();
      delete prev;

      if(preprev)
        preprev->Next = first;
    }
    else
    {
      preprev = prev;
    }
  }

  if(!(LastEntry = preprev))
    LastEntry = (struct ImageCacheItem *)&FirstEntry;
}

DecodeItem::DecodeItem (Object *obj, struct HTMLviewData *data, struct ImageList *image)
{
  Obj = obj;
  Data = data;
  Images = image;
  PageID = data->PageID;
  Thread = FindTask(NULL);
  InitSemaphore(&Mutex);
}

VOID DecodeItem::Start (struct PictureFrame *pic)
{
  Enter();
  Picture = pic;
  pic->LockPicture();
  Status = StatusDecoding;
  Leave();
}

BOOL DecodeItem::Update ()
{
  Enter();
  BOOL result = Status == StatusPending || Status == StatusDecoding;
  BOOL rem = Status == StatusDone || Status == StatusError;
  LONG y = CurrentY;
  ULONG pass = CurrentPass;
  BOOL done = Picture && Picture->Next;
  Leave();

  if(done)
  {
    done = LastY == (y = Picture->Height), pass = LastPass;

    if(!Anim && !Abort && PageID == Data->PageID)
    {
      Anim = Data->Share->AddAnim(Obj, Data, Picture, Images->Objects);
      Anim->Decode = this;
    }
  }

  if(Picture && !Abort && PageID == Data->PageID && !done)
  {
    if(rem)
    {
      y = Picture->Height;
      Picture->Flags |= PicFLG_Complete;
    }
    if(pass > 0 || y == Picture->Height)
      Picture->Flags |= PicFLG_Full;

    if(!Started)
    {
      Started = TRUE;

      BOOL relayout = FALSE;
      struct ObjectList *first = Images->Objects;
      while(first)
      {
        if(first->Obj->ReceiveImage(Picture))
          relayout = TRUE;
        first = first->Next;
      }

      if(relayout)
      {
        Data->Flags |= FLG_NotResized;
        if(DoMethod(Obj, MUIM_Group_InitChange))
        {
          Data->LayoutMsg.Reset(Data->Width, Data->Height);
          Data->HostObject->Relayout(TRUE);
          DoMethod(Obj, MUIM_Group_ExitChange);
        }
        Data->Flags &= ~FLG_NotResized;
      }
    }

    if(Picture->BMp)
    {
      LONG srcy = LastY;
      LastY = y;

      if(pass != LastPass)
      {
        if(y == Picture->Height)
            srcy = 0;
        else  LastY = 0, y = Picture->Height;
      }

      LONG top = Data->Top, bottom = Data->Top+Data->Height-1;

      struct ObjectList *first = Images->Objects;
      while(first)
      {
        if(y > srcy || first->Obj->id() != tag_IMG)
        {
          if(first->Obj->UpdateImage(srcy, y, top, bottom, rem))
          {
            Data->RedrawObj = first->Obj;
            MUI_Redraw(Obj, MADF_DRAWUPDATE);

            if(first->Obj->id() == tag_IMG)
            {
              class ImgClass *img = (class ImgClass *)first->Obj;
              img->YStart = 0;
              if(Picture->Flags & PicFLG_Full)
                img->YStop = Picture->Height;
            }
          }
        }
        first = first->Next;
      }
      LastPass = pass;
    }
  }

  if(rem)
  {
    if(Picture)
    {
      if(!Abort)
        Data->Share->ImageStorage->AddImage(Images->ImageName, Picture);
      Picture->UnLockPicture();
    }
    DecodeQueue->RemoveElm(this);
  }
  return(result);
}

/* This function is redundant, but keept for debugging purposes */
DecodeQueueManager::~DecodeQueueManager ()
{
/*  struct DecodeItem *prev, *first = Queue;
  while(first)
  {
    prev = first;
    first = first->Next;

    D(DBF_ALWAYS, "Status: %ld, Y: %ld, Pass: %ld, Started: %ld",
      prev->Status, prev->CurrentY, prev->CurrentPass, prev->Started);

    delete prev;
  }*/
}
VOID DecodeQueueManager::InsertElm (struct DecodeItem *item)
{
  ObtainSemaphore(&Mutex);
  if(Queue)
  {
    item->Next = Queue;
    Queue->Prev = item;
  }
  Queue = item;
  item->DecodeQueue = this;
  ReleaseSemaphore(&Mutex);
}

VOID DecodeQueueManager::RemoveElm (struct DecodeItem *item)
{
  ObtainSemaphore(&Mutex);
  if(item->Prev)
      item->Prev->Next = item->Next;
  else  Queue = item->Next;

  if(item->Next)
    item->Next->Prev = item->Prev;
  ReleaseSemaphore(&Mutex);

  if(item->Anim)
    item->Anim->Decode = NULL;

  delete item;
}

ULONG DecodeQueueManager::DumpQueue ()
{
  ULONG total = 0;
  ObtainSemaphore(&Mutex);
  struct DecodeItem *prev, *first = Queue;
//BOOL result = first ? TRUE : FALSE;
  while(first)
  {
    prev = first;
    first = first->Next;
    if(prev->Update())
    {
      total++;
    //result = FALSE;
    }
  }
  ReleaseSemaphore(&Mutex);
  return(total);
}

VOID DecodeQueueManager::InvalidateQueue (Object *obj)
{
  ObtainSemaphore(&Mutex);
  struct DecodeItem *first = Queue;
  while(first)
  {
    if(first->Obj == obj || !obj)
    {
      first->Enter();
      first->Abort = TRUE;
      if(first->Thread)
        Signal(first->Thread, SIGBREAKF_CTRL_C);
      first->Leave();
    }
    first = first->Next;
  }
  ReleaseSemaphore(&Mutex);
}

DISPATCHER(DecoderDispatcher)
{
  ULONG result = 0;
  struct DecoderData *data;

	//return 0;
  if (msg->MethodID!=OM_NEW)
		data = (struct DecoderData *)INST_DATA(cl, obj);

  switch(msg->MethodID)
  {
    case OM_NEW:
    {
      if((obj = (Object *)DoSuperMethodA(cl, obj, msg)))
      {
        struct DecoderData *data = (struct DecoderData *)INST_DATA(cl, obj);
        struct opSet *nmsg = (struct opSet *)msg;
        BOOL no_dither = FALSE;
        ULONG width = 0, height = 0;

        struct TagItem *tag, *tags = nmsg->ops_AttrList;
        while((tag = NextTagItem(&tags)))
        {
          LONG ti_Data = tag->ti_Data;
          switch(tag->ti_Tag)
          {
            case IDA_Screen:
              data->Scr = (struct Screen *)ti_Data;
            break;

            case IDA_NoDither:
              no_dither = ti_Data;
            break;

            case IDA_Width:
              width = ti_Data;
            break;

            case IDA_Height:
              height = ti_Data;
            break;

            case IDA_HTMLview:
              data->HTMLview = (Object *)ti_Data;
            break;

            case IDA_LoadHook:
              data->LoadHook = (struct Hook *)ti_Data;
            break;

            case IDA_LoadMsg:
              data->LoadMsg = (struct HTMLview_LoadMsg *)ti_Data;
            break;

            case IDA_StatusStruct:
              data->StatusItem = (struct DecodeItem *)ti_Data;
            break;

            case IDA_StartBuffer:
              data->StartBuffer = (UBYTE *)ti_Data;
            break;

            case IDA_BytesInBuffer:
              data->BytesInBuffer = ti_Data;
            break;

            case IDA_Gamma:
              data->Gamma = ti_Data;
            break;
          }
        }

        struct Screen *scr;
        if((scr = data->Scr))
        {
          ULONG depth = GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH);

          if(!CyberGfxBase && depth > 8)
            depth = 8;

          class ScaleEngine *img;
          if(depth >= 15)
              img = new (std::nothrow) TrueColourEngine(scr, width, height, data);
          else if(no_dither)
              img = new (std::nothrow) LowColourNDEngine(scr, width, height, data);
          else  img = new (std::nothrow) LowColourEngine(scr, width, height, data);

          if((data->ImgObj = img))
            return((ULONG)obj);
        }

        CoerceMethod(cl, obj, OM_DISPOSE);
      }
    }
    break;

    case IDM_Decode:
    {
      if((result = DoSuperMethodA(cl, obj, msg)))
        data->ImgObj->FlushBuffers();
    }
    break;

    case OM_DISPOSE:
    {
      delete data->ImgObj;
      result = DoSuperMethodA(cl, obj, msg);
    }
    break;

    case OM_GET:
    {
      struct opGet *gmsg = (struct opGet *)msg;

      switch(gmsg->opg_AttrID)
      {
        case IDA_Gamma:
        {
          if(data->Gamma)
          {
            *gmsg->opg_Storage = data->Gamma;
            result = TRUE;
          }
        }
        break;

        default:
          result = DoSuperMethodA(cl, obj, msg);
        break;
      }
    }
    break;

    case IDM_Read:
    {
      struct IDP_Read *rmsg = (struct IDP_Read *)msg;

      ULONG len = rmsg->Length;
      STRPTR buf = (STRPTR)rmsg->Buffer;
      if(data->BytesInBuffer)
      {
        result = MIN(len, data->BytesInBuffer);
        memcpy(buf, data->StartBuffer, result);
        buf += result;
        len -= result;
        data->StartBuffer += result;
        data->BytesInBuffer -= result;
      }

      data->LoadMsg->lm_Read.Buffer = buf;
      data->LoadMsg->lm_Read.Size = len;
      result += CallHookPkt(data->LoadHook, data->HTMLview, data->LoadMsg);
    }
    break;

    case IDM_Skip:
    {
      struct IDP_Skip *smsg = (struct IDP_Skip *)msg;
      UBYTE skip[512];
      LONG left = smsg->Length;

      if(data->BytesInBuffer)
      {
        LONG size = MIN((LONG)data->BytesInBuffer, left);
        left -= size;
        data->BytesInBuffer -= size;
      }

      while(left > 0)
      {
        data->LoadMsg->lm_Read.Buffer = (STRPTR)skip;
        data->LoadMsg->lm_Read.Size = left > 512 ? 512 : left;
        LONG sub = CallHookPkt(data->LoadHook, data->HTMLview, data->LoadMsg);
        if(!sub)
          break;
        left -= sub;
      }
    }
    break;

    case IDM_AllocateFrameTags:
    {
      struct IDP_AllocateFrameTags *smsg = (struct IDP_AllocateFrameTags *)msg;
      ULONG Interlaced = InterlaceNONE, AnimDelay = 0, Disposal = DisposeNOP, LeftOfs = 0, TopOfs = 0, Transparency = TransparencyNONE;
      struct RGBPixel *Background = NULL;

      struct TagItem *tag, *tags = &smsg->Tags;
      while((tag = NextTagItem(&tags)))
      {
        LONG ti_Data = tag->ti_Data;
        switch(tag->ti_Tag)
        {
          case AFA_Interlaced:
            Interlaced = ti_Data;
          break;

          case AFA_Transparency:
            Transparency = ti_Data;
          break;

          case AFA_LeftOfs:
            LeftOfs = ti_Data;
          break;

          case AFA_TopOfs:
            TopOfs = ti_Data;
          break;

          case AFA_AnimDelay:
            AnimDelay = ti_Data;
          break;

          case AFA_Disposal:
            Disposal = ti_Data;
          break;

          case AFA_Background:
            Background = (struct RGBPixel *)&ti_Data;
          break;
        }
      }

      result = data->ImgObj->SetDimensions(smsg->Width, smsg->Height, Interlaced, AnimDelay, Disposal, LeftOfs, TopOfs, Background, Transparency);
    }
    break;

    case IDM_SetDimensions:
    {
      struct IDP_SetDimensions *smsg = (struct IDP_SetDimensions *)msg;
      result = data->ImgObj->SetDimensions(smsg->Width, smsg->Height, smsg->Interlaced, smsg->AnimDelay, smsg->Disposal, smsg->LeftOfs, smsg->TopOfs, &smsg->Background, TransparencyNONE);
    }
    break;

    case IDM_GetLineBuffer:
    {
      struct IDP_GetLineBuffer *gmsg = (struct IDP_GetLineBuffer *)msg;
      result = (ULONG)data->ImgObj->GetLineBuffer(gmsg->Y, gmsg->LastPass);
    }
    break;

    case IDM_DrawLineBuffer:
    {
      struct IDP_DrawLineBuffer *dmsg = (struct IDP_DrawLineBuffer *)msg;
      data->ImgObj->DrawLineBuffer(dmsg->LineBuffer, dmsg->Y, dmsg->Height);
      result = data->StatusItem->Abort;
    }
    break;

    default:
    {
      result = DoSuperMethodA(cl, obj, msg);
    }
    break;
  }
  return(result);
}

struct Args
{
  Args (Object *obj, struct HTMLviewData *data, struct ImageList *image, struct Screen *scr, LONG sigbit, struct Task *parenttask)
  {
    TaskName = new (std::nothrow) char[strlen(image->ImageName)+12];
    if (TaskName)
    {
    	sprintf(TaskName, "HTMLview - %s", image->ImageName);
	    Name = TaskName + 11;
    }

    Obj = obj;
    Data = data;
    Img = image;
    Scr = scr;
    SigBit = sigbit;
    ParentTask = parenttask;
  }

  ~Args ()
  {
    UnlockPubScreen(NULL, Scr);
    delete TaskName;
  }

  Object *Obj;
  STRPTR TaskName, Name;
  struct ImageList *Img;
  struct Screen *Scr;
  struct HTMLviewData *Data;
  LONG SigBit;
  struct Task *ParentTask;
};

struct DecoderInfo
{
  CONST_STRPTR Name;
  BOOL (*MatchFunc)(UBYTE *);
  struct Library *Base;
  struct IClass *Class;
};

BOOL MatchGIF (UBYTE *x) { return(*((ULONG *)x) == MAKE_ID('G','I','F','8')); }
BOOL MatchJPG (UBYTE *x) { return(*((ULONG *)x) == 0xFFD8FFE0 && (*((ULONG *)(((UBYTE *)x) + 6)) == MAKE_ID('J','F','I','F'))); }
BOOL MatchPNG (UBYTE *x) { return(*((ULONG *)x) == 0x89504e47 && ((ULONG *)x)[1] == 0x0d0a1a0a); }
BOOL MatchDT  (UNUSED UBYTE *x) { return(TRUE); }
#define GIFDecoder "gif.decoder"
#define JPGDecoder "jfif.decoder"
#define PNGDecoder "png.decoder"
#define DTDecoder  "datatype.decoder"

struct DecoderInfo Decoders[] =
{
  { GIFDecoder, MatchGIF, NULL, NULL },
  { JPGDecoder, MatchJPG, NULL, NULL },
  { PNGDecoder, MatchPNG, NULL, NULL },
  { DTDecoder,  MatchDT,  NULL, NULL },
  { NULL,       NULL,     NULL, NULL }
};

CONST_STRPTR DecoderPaths[] =
{
  "MUI:Libs/MUI/HTMLview/",
  "LIBS:MUI/HTMLview/",
  "MUI:HTMLview/",
  "PROGDIR:Decoders/",
  "PROGDIR:MUI/HTMLview/",
  "LIBS:Decoders/",
  NULL
};

struct SignalSemaphore DecoderMutex;

static struct Library *ClassOpen(CONST_STRPTR decoder)
{
  struct Library *result;
  CONST_STRPTR *path = DecoderPaths;

  do {

    char name[64];
    strcpy(name, *path++);
    strcat(name, decoder);

    result = OpenLibrary(name, 0);

  } while(!result && *path);

  return(result);
}


Object *NewDecoderObjectA(UBYTE *buf,struct TagItem *attrs)
{
  struct TagItem *tags = attrs;
  struct IClass *cl = NULL;
  struct DecoderInfo *decoders = Decoders;

	//return 0;
  ObtainSemaphore(&DecoderMutex);

  while(decoders->Name && !cl)
  {
    if(decoders->MatchFunc(buf))
    {
      if(!(cl = decoders->Class))
      {
        if((decoders->Base = ClassOpen(decoders->Name)))
        {
          if((cl = MakeClass(NULL, NULL, GetImageDecoderClass(decoders->Base), sizeof(DecoderData), 0L)))
          {
            cl->cl_Dispatcher.h_SubEntry = 0;
            cl->cl_Dispatcher.h_Entry    = (ULONG(*)())ENTRY(DecoderDispatcher);
            cl->cl_Dispatcher.h_Data     = 0;

            decoders->Class = cl;
          }
        }
      }
    }
    decoders++;
  }

  ReleaseSemaphore(&DecoderMutex);

  if(cl)
  {
    return((Object *)NewObjectA(cl, NULL, tags));
  }

  else return(NULL);
}

extern "C" VOID _INIT_7_PrepareDecoders ();
VOID _INIT_7_PrepareDecoders ()
{
  memset(&DecoderMutex,0,sizeof(struct SignalSemaphore));
  InitSemaphore(&DecoderMutex);
}

extern "C" VOID _EXIT_7_FlushDecoders ();
VOID _EXIT_7_FlushDecoders ()
{
  struct DecoderInfo *decoders = Decoders;
  while(decoders->Name)
  {
    if(decoders->Class)
      FreeClass(decoders->Class);
    if(decoders->Base)
      CloseLibrary(decoders->Base);

    decoders++;
  }
}

VOID DecoderThread(REG(a0, STRPTR arguments))
{
  struct Args *args = (struct Args *)arguments;
  //sscanf(arguments, "%x", (unsigned int*)&args);

  BOOL result = FALSE;
  struct ImageList *image = args->Img;
  ULONG width = image->Width, height = image->Height;
  BOOL dither = args->Data->Share->DitherType;
  ULONG gamma = args->Data->Share->GammaCorrection;
  struct Hook *loadhook = args->Data->ImageLoadHook;
  struct HTMLview_LoadMsg loadmsg;
  loadmsg.lm_App = _app(args->Obj);

  struct DecodeItem *item = new (std::nothrow) struct DecodeItem(args->Obj, args->Data, image);
  if (item)
  {
    args->Data->Share->DecodeQueue.InsertElm(item);
    Signal(args->ParentTask, 1 << args->SigBit);

    loadmsg.lm_Type = HTMLview_Open;
    loadmsg.lm_PageID = item->PageID;
    loadmsg.lm_Open.URL = args->Name;
    loadmsg.lm_Open.Flags = MUIF_HTMLview_LoadMsg_Image;
    if(CallHookPkt(loadhook, args->Obj, &loadmsg))
    {
      UBYTE buf[12];
      loadmsg.lm_Type = HTMLview_Read;
      loadmsg.lm_Read.Buffer = (char *)buf;
      loadmsg.lm_Read.Size = 10;
      ULONG len = CallHookPkt(loadhook, args->Obj, &loadmsg);

  	  struct TagItem attrs[] =
       {{IDA_StartBuffer,  (ULONG)buf},
        {IDA_BytesInBuffer,(ULONG)len},
        {IDA_Width,        (ULONG)width},
        {IDA_Height,       (ULONG)height},
        {IDA_Screen,       (ULONG)args->Scr},
        {IDA_HTMLview,     (ULONG)args->Obj},
        {IDA_LoadHook,     (ULONG)loadhook},
        {IDA_LoadMsg,      (ULONG)&loadmsg},
        {IDA_StatusStruct, (ULONG)item},
        {IDA_NoDither,     (ULONG)dither},
        {IDA_Gamma,        (ULONG)gamma},
        {TAG_DONE,0}};
      Object *decoder = NewDecoderObjectA(buf,attrs);

      if(decoder)
      {
        result = DoMethod(decoder, IDM_Decode);
        DisposeObject(decoder);
      }

      loadmsg.lm_Type = HTMLview_Close;
      CallHookPkt(loadhook, args->Obj, &loadmsg);
    }

    delete args;

    item->Enter();
    item->Status = result ? StatusDone : StatusError;
    item->Thread = NULL;
    item->Leave();
  }
}

#if defined(__PPC__)
static const BOOL FBlit = FALSE;
#else
BOOL FBlit = FindPort("FBlit") ? TRUE : FALSE;
#endif

VOID DecodeImage (Object *obj, UNUSED struct IClass *cl, struct ImageList *image, struct HTMLviewData *data)
{
  LONG sigbit = 0;
  if(image && (sigbit = AllocSignal(-1)) > 0)
  {
    if(SetSignal(0L, 1 << sigbit) & (1 << sigbit))
    {
      DisplayBeep(NULL);
      W(DBF_STARTUP, "Signal was already pending!\n");
    }

    STRPTR name = NULL;
    struct List *pscrs = LockPubScreenList();
    for(struct Node *node = pscrs->lh_Head; node->ln_Succ; node = node->ln_Succ)
    {
      struct PubScreenNode *pnode = (struct PubScreenNode *)node;
      if(pnode->psn_Screen == _screen(obj))
      {
        name = pnode->psn_Node.ln_Name;
        break;
      }
    }
    UnlockPubScreenList();
    struct Screen *lock = LockPubScreen(name);

    struct Args *args = new (std::nothrow) struct Args(obj, data, image, lock, sigbit, FindTask(NULL));
    if (!args)
    {
        UnlockPubScreen(NULL,lock);
        return;
    }
    char str_args[10];
    sprintf(str_args, "%lx", (ULONG)args);

    STRPTR taskname = FBlit ? (char *)"HTMLview ImageDecoder" : args->TaskName;
    if(CreateNewProcTags(
      NP_Entry,        (ULONG)DecoderThread,
      NP_Priority,     (ULONG)-1,
      NP_Name,         (ULONG)taskname,
      #if defined(__MORPHOS__)
      NP_PPC_Arg1,     (ULONG)args,
      NP_CodeType, 	   CODETYPE_PPC,
	    NP_PPCStackSize, STACKSIZEPPC,
      NP_StackSize,    STACKSIZE68K,
	    NP_CopyVars,     FALSE,
      NP_Input,        NULL,
      NP_CloseInput,   FALSE,
      NP_Output,       NULL,
	    NP_CloseOutput,  FALSE,
      NP_Error,        NULL,
      NP_CloseError,   FALSE,
      #endif
      TAG_DONE))
    {
      Wait(1 << sigbit);
    }

    FreeSignal(sigbit);
  }
  else
  {
    DisplayBeep(NULL);
    W(DBF_STARTUP, "No image! - %s (%ld)\n", image->ImageName, sigbit);
  }
}
