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
#include <clib/alib_protos.h>
#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
//#include <datatypes/pictureclassext.h>
#include <dos/dostags.h>
#include <graphics/gfxmacros.h>
#include <graphics/rpattr.h>
#include <intuition/classusr.h>
#include <libraries/mui.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
//#include <mui/ipc_mcc.h>

#include "General.h"
#include "ParseThread.h"
#include "ParseMessage.h"
#include "Layout.h"
#include "Render.h"
#include "Classes.h"
#include "GetImages.h"
#include "ScrollGroup.h"
#include "ImageManager.h"
#include "Animation.h"
#include "IM_Render.h"
#include "SharedData.h"
#include "URLHandling.h"

#include "classes/AClass.h"
#include "classes/HostClass.h"
#include "classes/ImgClass.h"

#include "private.h"
#include <stdio.h>

#include "Debug.h"

//#undef NewObject
//extern "C" APTR NewObject ( struct IClass *classPtr , STRPTR classID , ...);
//#undef MUI_NewObject

BOOL mSet (Object *obj, struct IClass *cl, struct opSet *msg);
ULONG mGet (Object *obj, struct IClass *cl, struct opGet *msg);

LONG MUIG_ScrollRaster(struct RastPort *rp, WORD dx, WORD dy, WORD left, WORD top, WORD right, WORD bottom);

#define MAX_HEIGHT 0x0fffffff
#define IMAGE_QUEUE_SIZE 8

VOID MUI_PulseNode::Start (Object *obj)
{
  if(!Running)
    DoMethod(_app(obj), MUIM_Application_AddInputHandler, (ULONG)this);
  Running = TRUE;
}

BOOL MUI_PulseNode::Stop (Object *obj)
{
  BOOL res;
  if((res = Running))
    DoMethod(_app(obj), MUIM_Application_RemInputHandler, (ULONG)this);
  Running = FALSE;
  return(res);
}

HOOKPROTONHNO(DefaultLoadFunc, ULONG, struct HTMLview_LoadMsg* lmsg)
{
  switch(lmsg->lm_Type)
  {
    case HTMLview_Open:
    {
      STRPTR file = lmsg->lm_Params.lm_Open.URL+7;
      APTR result = lmsg->lm_Userdata = (APTR)Open(file, MODE_OLDFILE);
      return((ULONG)result);
    }
    break;

    case HTMLview_Read:
    {
      APTR fh = lmsg->lm_Userdata; STRPTR buf = lmsg->lm_Params.lm_Read.Buffer; LONG len = lmsg->lm_Params.lm_Read.Size;
      len = Read((BPTR)fh, buf, len);
      return(len > 0 ? len : 0);
    }
    break;

    case HTMLview_Close:
    {
      APTR fh = lmsg->lm_Userdata;
      Close((BPTR)fh);
    }
    break;
  }

  return 0;
}
MakeStaticCppHook(DefaultLoadHook, DefaultLoadFunc);

VOID RemoveChildren (Object *group, struct HTMLviewData *data)
{
  data->Flags |= FLG_NoBackfill;
  class HostClass *host = data->HostObject;
  data->HostObject = NULL;
  if(DoMethod(group, MUIM_Group_InitChange))
  {
    DoMethod(group, MUIM_HTMLview_RemoveChildren);
    data->TopChange = MAX_HEIGHT-10;
    DoMethod(group, MUIM_Group_ExitChange);
  }
  data->HostObject = host;
}

extern struct MUI_CustomClass  *ThisClass;

HOOKPROTONH(LayoutCode, ULONG, Object *obj, struct MUI_LayoutMsg *lmsg)
{
  ULONG result;

  ENTER();

  switch(lmsg->lm_Type)
  {
    case MUILM_MINMAX:
    {
      lmsg->lm_MinMax.MinWidth  = 1;
      lmsg->lm_MinMax.MinHeight = 1;
      lmsg->lm_MinMax.MaxWidth  = MUI_MAXMAX;
      lmsg->lm_MinMax.MaxHeight = MUI_MAXMAX;
      lmsg->lm_MinMax.DefWidth  = 100;
      lmsg->lm_MinMax.DefHeight = 100;
    }
    break;

    case MUILM_LAYOUT:
    {
      struct HTMLviewData *data = (struct HTMLviewData *)INST_DATA(ThisClass->mcc_Class,obj);

      BOOL frames = (data->Height != lmsg->lm_Layout.Height && data->HostObject && data->HostObject->Body && data->HostObject->Body->id() == tag_FRAMESET);

      data->Height = lmsg->lm_Layout.Height;

      if(data->Width != lmsg->lm_Layout.Width || frames || data->Share->Flags & FLG_NewConfig)
      {
        data->Width = lmsg->lm_Layout.Width;
        data->LayoutMsg.Reset(data->Width, data->Height);
        if(data->HostObject)
          data->HostObject->Relayout(data->Share->Flags & FLG_NewConfig);
        data->Share->Flags &= ~FLG_NewConfig;
      }

      data->LayoutMsg.TopChange = MAX_HEIGHT;
      if(data->HostObject)
      {
        data->HostObject->Layout(data->LayoutMsg);
        if(data->Flags & FLG_ReAllocColours)
          data->HostObject->AdjustPosition(0, 0);
        data->Flags &= ~FLG_ReAllocColours;
      }

      if(data->Flags & FLG_NotResized && data->LayoutMsg.TopChange == MAX_HEIGHT)
        data->LayoutMsg.TopChange -= 10;
      data->TopChange = data->LayoutMsg.TopChange;

      struct GadgetList *first = data->LayoutMsg.FirstGadget;
      while(first)
      {
        first->GadgetObject->AdjustPosition(0, 0);
        first = first->Next;
      }

      lmsg->lm_Layout.Width = data->VirtWidth = data->LayoutMsg.Width;
      lmsg->lm_Layout.Height = data->VirtHeight = data->LayoutMsg.Height;

      result = TRUE;
    }
    break;
  }

  RETURN(result);
  return result;
}
MakeStaticCppHook(LayoutHook, LayoutCode);

struct alfieMsg
{
    struct Message link;
    struct ParseThreadArgs *args;
};

/*
#define NewRawDoFmt(__p0, __p1, __p2, ...) \
	(((STRPTR (*)(void *, CONST_STRPTR , APTR (*)(APTR, UBYTE), STRPTR , ...))*(void**)((long)(EXEC_BASE_NAME) - 922))((void*)(EXEC_BASE_NAME), __p0, __p1, __p2, __VA_ARGS__))
*/

CPPDISPATCHER(_Dispatcher)
{
  ULONG result = 0;
  struct HTMLviewData *data;

  if (msg->MethodID!=OM_NEW)
	data = (struct HTMLviewData *)INST_DATA(cl, obj);

  //kprintf("HTMLview: cl %08lx obj %08lx Method = 0x%08lx\n", cl, obj, msg->MethodID);
  switch(msg->MethodID)
  {
    case OM_NEW:
    {
      ENTER();

      struct opSet *nmsg = (struct opSet *)msg;

      struct TagItem tags[] =
      {
        { MUIA_CustomBackfill, TRUE },
//        { MUIA_Background, MUII_SHADOWFILL },
        { MUIA_FillArea, FALSE },
        { MUIA_InnerBottom, 0 },
        { MUIA_InnerLeft, 0 },
        { MUIA_InnerRight, 0 },
        { MUIA_InnerTop, 0 },
		//#define MUIA_DoubleBufferClipped 0x8042f282
        //{MUIA_DoubleBufferClipped, 1},
        { TAG_MORE, (ULONG)nmsg->ops_AttrList }
      };

      D(DBF_STARTUP, "before DoSuperMethod");

      if((obj = (Object *)DoSuperMethod(cl, obj, OM_NEW, (ULONG)tags, (ULONG)nmsg->ops_GInfo)))
      {
        struct HTMLviewData *data = (struct HTMLviewData *)INST_DATA(cl, obj);

        D(DBF_STARTUP, "after DoSuperMethod");

		//kprintf("HTMLVIEW OBJECT %lx\n",obj);
        data->ImageLoadHook = data->LoadHook = &DefaultLoadHook;

        //InitHook(&LayoutHook, LayoutHook, data);
        //LayoutHook.h_Data = (APTR)data;
        SetAttrs(obj, MUIA_Group_LayoutHook, (ULONG)&LayoutHook, TAG_DONE);

        D(DBF_STARTUP, "after DoSuperMethod1");

        data->LayoutMsg.HTMLview = data->RenderMsg.HTMLview = obj;
        data->LayoutMsg.MarginWidth = data->LayoutMsg.MarginHeight = 5;

        data->Flags |= FLG_DiscreteInput;

        D(DBF_STARTUP, "after DoSuperMethod2");

        struct TagItem *tag;
        if((tag = FindTagItem(MUIA_HTMLview_SharedData, nmsg->ops_AttrList)))
        {
          D(DBF_STARTUP, "after DoSuperMethod3");
          data->Share = (struct SharedData *)tag->ti_Data;
        }
        else
        {
          D(DBF_STARTUP, "after DoSuperMethod4");
          data->Share = new (std::nothrow) struct SharedData(cl, obj, data);
          if (!data->Share)
          {
            CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
            return 0;
          }
        }

        D(DBF_STARTUP, "after DoSuperMethod5");

        data->LayoutMsg.Share = data->RenderMsg.Share = data->Share;

        D(DBF_STARTUP, "before mSet()");

        mSet(obj, cl, nmsg);

        D(DBF_STARTUP, "before CreateMsgPort!");


        int sigBit;

        if((tag = FindTagItem(MUIA_HTMLview_SigBit,nmsg->ops_AttrList)) != NULL)
        {
		  sigBit = (int)tag->ti_Data;
		  data->SigBit = -1;
        }
        else
        {
          data->SigBit = sigBit = AllocSignal(-1);
        }

        if (sigBit>0)
        {
          data->MessagePort.mp_Flags   = PA_SIGNAL;
          data->MessagePort.mp_SigBit  = (UBYTE)sigBit;
          data->MessagePort.mp_SigTask = FindTask(NULL);
          NewList(&data->MessagePort.mp_MsgList);

          D(DBF_STARTUP, "MsgPort created.");

		  //kprintf("MSGPORT %lx %lx\n",obj,data->MessagePort);
          data->ihnode.ihn.ihn_Object  = obj;
          data->ihnode.ihn.ihn_Signals = 1 << sigBit;
          data->ihnode.ihn.ihn_Flags   = 0L;
          data->ihnode.ihn.ihn_Method  = MUIM_HTMLview_ExtMessage;

          sprintf(data->ParseThreadName, "HTMLview ParseThread 0x%08lx", (ULONG)obj);

          if((tag = FindTagItem(MUIA_HTMLview_Scrollbars, nmsg->ops_AttrList)))
          {
            D(DBF_STARTUP, "creating scollgroupclass object...");

            return (ULONG)NewObject(ScrollGroupClass->mcc_Class, NULL,
                                    MUIA_ScrollGroup_Scrolling,  tag->ti_Data,
                                    MUIA_ScrollGroup_HTMLview,   obj,
                                    TAG_DONE);
          }
          else
          {
            D(DBF_STARTUP, "created HTMLview object.");

            RETURN(obj);
            return((ULONG)obj);
          }
        }
        else
        {
		  D(DBF_STARTUP, "MsgPort NOT Created!!");
          CoerceMethod(cl, obj, OM_DISPOSE);
        }
      }
    }
    break;

    case OM_DISPOSE: { ENTER();

      // This deletes notifies and floading images (should be in ~LayoutMessage()) */
      //kprintf("--------- DISPOSE 1 %lx\n",obj);
      data->LayoutMsg.Reset(0, 0);
      //kprintf("--------- DISPOSE 2 %lx\n",obj);

      if(data->ParseThread) Signal(&data->ParseThread->pr_Task, SIGBREAKF_CTRL_C);

      //kprintf("--------- DISPOSE 3 %lx\n",obj);

      while(data->ParseCount)
      {
        struct ParseInfoMsg *msg;

        //kprintf("--------- DISPOSE 4 %lx %ld\n",obj,data->ParseCount);
		msg = (struct ParseInfoMsg *)GetMsg(&data->MessagePort);
        if (msg)
        {
          switch(msg->Class)
          {
            case ParseMsg_Startup:
            {
              if(data->PageID == msg->Unique)
              {
                if(data->Flags & FLG_HostObjNotUsed)
                {
                  RemoveChildren(obj, data);
                  delete data->HostObject;
                  data->Flags &= ~FLG_HostObjNotUsed;
                }
                data->HostObject = msg->Startup.Object;
              }
            }
            break;

            case ParseMsg_Shutdown:
            {
              if(data->PageID == msg->Unique)
              {
                data->Period.Stop(obj);
              }
              else
              {
                if(data->HostObject != msg->Shutdown.Object)
                  delete msg->Shutdown.Object;
              }
              data->ParseCount--;
            }
            break;

            case ParseMsg_Abort:
            {
              data->ParseCount--;
            }
            break;
          }
          ReplyMsg(&msg->nm_node);
          //kprintf(" --------- DISPOSE 4.1 Message  %lx %ld\n",obj,data->ParseCount);
        }
        else WaitPort(&data->MessagePort);
      }

      //kprintf("--------- DISPOSE 5 %lx\n",obj);
      while(FindTask(data->ParseThreadName))
      {
        //kprintf("--------- DISPOSE 6 %lx\n",obj);
        Delay(1);
	  }

      if (data->SigBit>0)
        FreeSignal(data->SigBit);

      //kprintf("--------- DISPOSE 7 %lx\n",obj);

      RemoveChildren(obj, data);

      //kprintf("--------- DISPOSE 8 %lx\n",obj);
      delete data->LayoutMsg.FirstGadget;
      delete data->HostObject;
      delete data->URLBase;
      delete data->Page;
      delete data->Local;
      delete data->URL;

      //kprintf("--------- DISPOSE 9 %lx\n",obj);

      struct MUIR_HTMLview_GetContextInfo *cinfo = &data->ContextInfo;
      delete cinfo->URL;
      delete cinfo->Img;
      delete cinfo->Frame;
      delete cinfo->Background;

      if(data->Flags & FLG_RootObj)
        delete data->Share;

      //kprintf("--------- DISPOSE 10 %lx\n",obj);
      result = DoSuperMethodA(cl, obj, (Msg)msg);
      //kprintf("--------- DISPOSE 11 %lx\n",obj);
    }
    break;

    case MUIM_AskMinMax:
    {
      ENTER();

      struct MUIP_AskMinMax *amsg = (struct MUIP_AskMinMax *)msg;
      result = DoSuperMethodA(cl, obj, (Msg)msg);

      amsg->MinMaxInfo->MinWidth  = _subwidth(obj)+1;;
      amsg->MinMaxInfo->MinHeight = _subheight(obj)+1;
      amsg->MinMaxInfo->DefWidth  = 200;
      amsg->MinMaxInfo->DefHeight = 200;
      amsg->MinMaxInfo->MaxWidth  = MBQ_MUI_MAXMAX;
      amsg->MinMaxInfo->MaxHeight = MBQ_MUI_MAXMAX;
    }
    break;

    case MUIM_HTMLview_RemoveChildren:
    {
      ENTER();

      data->LayoutMsg.FlushGadgetList();

      Object *child;
      struct List *childs = (struct List *)xget(obj, MUIA_Group_ChildList);
      Object *head = (Object *)childs->lh_Head;

      while((child = (Object *)NextObject(&head)))
      {
        DoMethod(obj, OM_REMMEMBER, (ULONG)child);
        MUI_DisposeObject(child);
      }
    }
    break;

    case OM_SET:
    {
      ENTER();

      struct opSet *smsg = (struct opSet *)msg;
      ULONG forward = mSet(obj, cl, smsg);

      struct TagItem newtags[] =
      {
        { MUIA_Group_Forward, forward },
        { TAG_MORE, (ULONG)smsg->ops_AttrList }
      };
      result = DoSuperMethod(cl, obj, OM_SET, (ULONG)newtags, (ULONG)smsg->ops_GInfo);
    }
    break;

    case OM_GET:
    {
      ENTER();
      result = mGet(obj, cl, (struct opGet *)msg);
    }
    break;

    case MUIM_Draw:
    {
      ENTER();
      struct MUIP_Draw *dmsg = (struct MUIP_Draw *)msg;

      result = DoSuperMethodA(cl, obj, (Msg)msg);

      if(dmsg->flags & MADF_DRAWUPDATE)
      {
        LONG minx, miny, maxx, maxy;
        minx = data->XOffset;
        miny = data->YOffset;
        maxx = data->XOffset+data->Width-1;
        maxy = data->YOffset+data->Height-1;
        LONG xoffset = data->Left - data->XOffset, yoffset = data->Top - data->YOffset;

        APTR cliphandle = MUI_AddClipping(muiRenderInfo(obj), minx, miny, maxx-minx+1, maxy-miny+1);

        if(data->Flags & FLG_RedrawMarked)
        {
          data->HostObject->MarkMsg.DrainBoxList(_rp(obj), xoffset, yoffset);
        }
        else
        {
          data->RenderMsg.Reset(minx, miny, maxx, maxy, xoffset, yoffset, data->Left + (minx - data->XOffset), data->Top + (miny - data->YOffset), data->Share->Pens);
          data->RenderMsg.TargetObj = data->RedrawObj;
          if(data->Flags & FLG_ActiveLink)
            data->RenderMsg.Textstyles |= TSF_ALink;

          data->HostObject->Render(data->RenderMsg);
          data->Flags &= ~FLG_ActiveLink;
        }

        MUI_RemoveClipping(muiRenderInfo(obj), cliphandle);
      }
    }
    break;

    case MUIM_Backfill:
    {
      ENTER();
      struct MUIP_Backfill *bmsg = (struct MUIP_Backfill *)msg;

      /*
      **  This is to fix but this is hard. I have no time.
      **  This is the hack that stuntzi uses to make it work
      **  otherwise it draws itself outside its box
      **  bad, bad, bad. It is also the cause of the links
      **  problem, I suppose.
      */

      D(DBF_ALWAYS, "HTMLview: MUIM_Backfill");
	  {

	  	LONG l, r, t, b;

        l = bmsg->left;
        r = bmsg->right;
        t = bmsg->top;
        b = bmsg->bottom;

	  	if (l>r || t>b) return 0;

  		if (!(l==_mleft(obj) && t==_mtop(obj) && r==_mright(obj) && b==_mbottom(obj)))
        {
	        D(DBF_ALWAYS, "HTMLview: %ld %ld, %ld %ld, %ld %ld, %ld %ld",
  		  	l,_mleft(obj),t,_mtop(obj),r,_mright(obj),b,_mbottom(obj));

            return 0;
          }
	  }

      struct RastPort *rp = _rp(obj);

      data->Left = xget(obj, MUIA_Virtgroup_Left);
      data->Top = xget(obj, MUIA_Virtgroup_Top);

      SHOWVALUE(DBF_ALWAYS, data->HostObject);
      if(data->HostObject)
        SHOWVALUE(DBF_ALWAYS, data->HostObject->Body);
      if(!data->HostObject || (!data->HostObject->Body && (data->Flags & FLG_HostObjNotUsed)))
      {
        if(!(data->Flags & FLG_NoBackfill))
        {
          D(DBF_ALWAYS, "HTMLview: no nobackfill");
          data->Flags &= ~FLG_NoBackfill;
          SetAPen(rp, data->Share->Pens[Col_Background]);
          RectFill(rp, data->XOffset, data->YOffset, data->XOffset+data->Width-1, data->YOffset+data->Height-1);
        }
      }
      else
      {
        LONG top = 0;
        if(data->TopChange && data->TopChange != MAX_HEIGHT)
        {
          top = data->TopChange-data->Top;
          data->TopChange = 0;
        }

        if(top < (LONG)data->Height)
        {
          APTR Cliphandle = NULL;

          if(top)
            Cliphandle = MUI_AddClipping(muiRenderInfo(obj), data->XOffset, data->YOffset+top, data->Width, data->Height);

/*
          LONG dx = data->HScrollDiff, dy = data->VScrollDiff;
          if(MUIMasterBase->lib_Version < 220 && (dx || dy))
          {
//            DoSuperMethodA(cl, obj, msg);
            LONG minx, miny, width, height;

            if(dx < 0)
            {
              minx = data->XOffset;
              width = -dx;
            }
            else
            {
              minx = data->XOffset+data->Width-dx;
              width = dx;
            }

            if(dy < 0)
            {
              miny = data->YOffset;
              height = -dy;
            }
            else
            {
              miny = data->YOffset+data->Height-dy;
              height = dy;
            }

            struct Hook *oldhook = InstallLayerHook(rp->Layer, LAYERS_NOBACKFILL);
            ScrollRasterBF(rp, dx, dy, data->XOffset, data->YOffset, data->XOffset+data->Width-1, data->YOffset+data->Height-1);
            InstallLayerHook(rp->Layer, oldhook);
//            MUIG_ScrollRaster(rp, dx, dy, data->XOffset, data->YOffset, data->XOffset+data->Width-1, data->YOffset+data->Height-1);

            if(dx)
            {
              APTR cliphandle = MUI_AddClipping(muiRenderInfo(obj), minx, data->YOffset, width, data->Height);
              LONG miny = bmsg->top;
              LONG maxx = minx + width - 1;
              LONG maxy = bmsg->bottom;
              data->RenderMsg.Reset(minx, miny, maxx, maxy, data->Left-data->XOffset, data->Top-data->YOffset, data->Left + (minx - data->XOffset), data->Top + (miny - data->YOffset), data->Share->Pens);
              data->HostObject->Render(data->RenderMsg);
              MUI_RemoveClipping(muiRenderInfo(obj), cliphandle);
            }

            if(dy)
            {
              APTR cliphandle = MUI_AddClipping(muiRenderInfo(obj), data->XOffset, miny, data->Width, height);
              minx = bmsg->left;
              LONG maxx = bmsg->right;
              LONG maxy = miny + height - 1;
              data->RenderMsg.Reset(minx, miny, maxx, maxy, data->Left-data->XOffset, data->Top-data->YOffset, data->Left + (minx - data->XOffset), data->Top + (miny - data->YOffset), data->Share->Pens);
              data->HostObject->Render(data->RenderMsg);
              MUI_RemoveClipping(muiRenderInfo(obj), cliphandle);
            }
            data->HScrollDiff = data->VScrollDiff = 0;
          }
          else

*/
	      {
            struct Rectangle rect;
            GetRPAttrs(rp, RPTAG_DrawBounds, (ULONG)&rect, TAG_DONE);
            LONG minx = rect.MinX, miny = rect.MinY, maxx = rect.MaxX, maxy = rect.MaxY;
            //kprintf("%ld, %ld -- %ld, %ld\n", minx, maxx, miny, maxy);
            if(minx > maxx)
            {
              minx = bmsg->left;
              miny = bmsg->top;
              maxx = bmsg->right;
              maxy = bmsg->bottom;
            }

	        /*NewRawDoFmt("%ld %ld %ld %ld %ld %ld %ld %ld\n",(void * (*)(void *, UBYTE))1,NULL,
				bmsg->left,  minx,
				bmsg->right, maxx,
				bmsg->top,   miny,
				bmsg->bottom,maxy);*/

            D(DBF_ALWAYS, "HTMLview: render");
            data->RenderMsg.Reset(minx, miny, maxx, maxy, data->Left-data->XOffset, data->Top-data->YOffset, data->Left + (minx - data->XOffset), data->Top + (miny - data->YOffset), data->Share->Pens);
            data->HostObject->Render(data->RenderMsg);
          }

          if(Cliphandle)
            MUI_RemoveClipping(muiRenderInfo(obj), Cliphandle);
        }
      }
    }
    break;

    case MUIM_Setup:
    {
      ENTER();

      struct MUIP_Setup *smsg = (struct MUIP_Setup *)msg;

      struct Screen *scr = smsg->RenderInfo->mri_Screen;
      if(data->Flags & FLG_RootObj)
        data->Share->Setup(scr);

      SetAttrs(obj,
        MUIA_HTMLview_Prop_HDeltaFactor, data->Share->HDeltaFactor,
        MUIA_HTMLview_Prop_VDeltaFactor, data->Share->VDeltaFactor,
        TAG_DONE);

      if((result = DoSuperMethodA(cl, obj, (Msg)msg)))
      {
        data->ihnode.Start(obj);

        data->Period.ihn.ihn_Object = obj;
        data->Period.ihn.ihn_Method = MUIM_HTMLview_Period;
        data->Period.ihn.ihn_Flags  = MUIIHNF_TIMER;
        data->Period.ihn.ihn_Millis = 100;

        if(data->Flags & FLG_ReAllocColours)
        {
          data->HostObject->AllocateColours(scr->ViewPort.ColorMap);
          data->Flags &= ~FLG_ReAllocColours;
        }

        if(data->HostObject)
        {
          struct GetImagesMessage gmsg(obj);
          data->HostObject->GetImages(gmsg);
          data->Images = gmsg.Images;
          DoMethod(obj, MUIM_HTMLview_LoadImages, (ULONG)gmsg.Images);
        }

        if(data->Share->Obj == obj)
        {
          DoMethod(_win(obj), MUIM_Notify, MUIA_Window_Activate, FALSE, (ULONG)obj, 1, MUIM_HTMLview_PauseAnims);
          DoMethod(_win(obj), MUIM_Notify, MUIA_Window_Activate, TRUE,  (ULONG)obj, 1, MUIM_HTMLview_ContinueAnims);
        }
      }
    }
    break;

    case MUIM_Cleanup:
    {
      ENTER();

      if(data->Share->Obj == obj)
        DoMethod(_win(obj), MUIM_KillNotifyObj, MUIA_Window_Activate, (ULONG)obj);

      if(data->HostObject)
      {
        data->HostObject->FreeColours(data->Share->Scr->ViewPort.ColorMap);
        data->Flags |= FLG_ReAllocColours;
      }

      data->RenderMsg.FlushDoubleBuffer();

      delete data->Images;
      data->Images = data->ImagesLeft = NULL;
      data->RunningDecoderThreads = 0;

      data->ihnode.Stop(obj);

      data->Share->DecodeQueue.InvalidateQueue(obj);
      data->Share->StopAnims(obj);

      result = DoSuperMethodA(cl, obj, (Msg)msg);

      if(data->Flags & FLG_RootObj)
        data->Share->Cleanup();
    }
    break;

    case MUIM_Show:
    {
      ENTER();

    //struct MUIP_Show *smsg = (struct MUIP_Show *)msg;
      struct MUI_AreaData *ad = muiAreaData(obj);

      data->XOffset = ad->mad_Box.Left + ad->mad_addleft;
      data->YOffset = ad->mad_Box.Top + ad->mad_addtop;

      data->RenderMsg.RPort = _rp(obj);

      if(data->Width >= data->VirtWidth)
        data->Left = 0;
      else if(data->Left + data->Width > data->VirtWidth)
        data->Left = data->VirtWidth - data->Width;

      if(data->Height >= data->VirtHeight)
        data->Top = 0;
      else if(data->Top + data->Height > data->VirtHeight)
        data->Top = data->VirtHeight - data->Height;

      SetAttrs(obj,
        MUIA_Virtgroup_Left,        data->Left,
        MUIA_Virtgroup_Top,       data->Top,
        MUIA_Width,             data->Width,
        MUIA_Height,            data->Height,
        MUIA_Virtgroup_Width,     data->VirtWidth,
        MUIA_Virtgroup_Height,      data->VirtHeight,
        TAG_DONE);

      data->Flags |= FLG_Shown;
      if(!(data->Flags & FLG_NotResized))
      {
        //SetAttrs(_win(obj), MUIA_Window_Sleep, FALSE, TAG_DONE);
      }

      result = DoSuperMethodA(cl, obj, (Msg)msg);
    }
    break;

    case MUIM_Hide:
    {
      ENTER();

      data->Flags &= ~FLG_Shown;
      if(!(data->Flags & FLG_NotResized))
      {
        //SetAttrs(_win(obj), MUIA_Window_Sleep, TRUE, TAG_DONE);
      }

      result = DoSuperMethodA(cl, obj, (Msg)msg);
    }
    break;

    case MUIM_HTMLview_HandleEvent:
    case MUIM_HandleEvent:
    {
      ENTER();

      if(data->HostObject && data->HostObject->Body && ((struct MUIP_HandleEvent *)msg)->imsg && (data->Flags & FLG_Shown))
        data->HostObject->HandleEvent(obj, cl, (struct MUIP_HandleEvent *)msg);
    }
    break;

    case MUIM_HTMLview_Abort:
    {
      ENTER();

      if(data->ParseThread) Signal(&data->ParseThread->pr_Task, SIGBREAKF_CTRL_C);
      data->Share->DecodeQueue.InvalidateQueue(NULL);
    }
    break;

    case MUIM_HTMLview_AbortAll:
    {
      ENTER();

      data->PageID++;
      //kprintf("ABORTALL %lx %lx %ld\n",obj,data->ParseThread,data->ParseCount);
      if(data->ParseThread)
      {
        Signal(&data->ParseThread->pr_Task, SIGBREAKF_CTRL_C);
        data->PMsg = NULL;
        //alfie: data->ParseThread->pr_Task.tc_Node.ln_Pri--;
      }

      delete data->Images;
      data->Images = data->ImagesLeft = NULL;
      data->RunningDecoderThreads = 0;

      data->Share->DecodeQueue.InvalidateQueue(obj);
      data->Share->StopAnims(obj);
    }
    break;

    case MUIM_HTMLview_PrivateGotoURL:
    {
      ENTER();

      struct MUIP_HTMLview_GotoURL *gmsg = (struct MUIP_HTMLview_GotoURL *)msg;

      data->Flags |= FLG_NoBackfill;
      STRPTR url = gmsg->URL;
      result = CoerceMethod(cl, obj, MUIM_HTMLview_GotoURL, (ULONG)url, (ULONG)gmsg->Target);
      delete url;
    }
    break;

    case MUIM_HTMLview_Reload:
    {
      ENTER();

      data->Flags |= FLG_Reload;
      data->Share->ImageStorage->FlushCache((STRPTR)MUIV_HTMLview_FlushImage_Displayed);
      DoMethod(obj, MUIM_HTMLview_AbortAll);
      result = data->PageID;

      if(data->URLBase && data->Page)
        DoMethod(obj, MUIM_HTMLview_StartParser);

      data->Flags &= ~FLG_Reload;
    }
    break;

    case MUIM_HTMLview_GotoURL:
    {
      ENTER();

      struct MUIP_HTMLview_GotoURL *gmsg = (struct MUIP_HTMLview_GotoURL *)msg;

      /*kprintf("GoToURL got for %lx (%s) %lx (%s)\n",
      	  gmsg->Target,
	      gmsg->Target ? gmsg->Target : "",
    	  data->FrameName,
    	  data->FrameName ? data->FrameName : "");*/

      if(gmsg->Target && gmsg->Target != data->FrameName)
      {
        Object *dst;

        if((dst = (Object *)DoMethod(obj, MUIM_HTMLview_LookupFrame, (ULONG)gmsg->Target)))
        {
          if(dst != obj)
          {
            DoMethodA(dst, (Msg)msg);
            return(data->PageID);
          }
          else
            D(DBF_STARTUP, "%s == %s (0x%lx, 0x%lx)\n", gmsg->Target, data->FrameName, gmsg->Target, data->FrameName);
        }
      }

      STRPTR url = gmsg->URL,
        tmpstr = strchr(url, ':') ? NULL : (url = (STRPTR)DoMethod(obj, MUIM_HTMLview_AddPart, (ULONG)url));

      ULONG len = strlen(url);
      if(url && len > 7)
      {
        delete data->URL;
        data->URL = new (std::nothrow) char[len+1];
        if (!data->URL) return 0;
        strcpy(data->URL, url);

        ULONG baselen, pagelen;
        if(data->URLBase && !strncmp(data->URLBase, url, (baselen = strlen(data->URLBase))) && !strncmp(data->Page, url+baselen, (pagelen = strlen(data->Page))))
        {
          if(url[baselen+pagelen] == '#')
          {
            class AClass *anchor;
            if(data->HostObject && (anchor = data->HostObject->FindAnchor(url+baselen+pagelen+1)))
            {
              LONG top = (anchor->top() > 5) ? anchor->top()-5 : 0;
              if(top != data->Top)
                SetAttrs(obj, MUIA_Virtgroup_Top, top, TAG_DONE);
            }
            else
            {
              if(!(data->Flags & FLG_HostObjNotUsed))
              {
                delete data->Local;
                data->Local = new (std::nothrow) char[strlen(url+baselen+pagelen)];
                if (!data->Local) return 0;
                strcpy(data->Local, url+baselen+pagelen+1);
              }
            }
            delete tmpstr;
            return(data->PageID);
          }
          else if(url[baselen+pagelen] == '\0')
          {
            SetAttrs(obj, MUIA_Virtgroup_Top, 0, TAG_DONE);
            delete tmpstr;
            return(data->PageID);
          }
        }

        DoMethod(obj, MUIM_HTMLview_AbortAll);
        data->Flags &= ~FLG_NoBackfill;
        result = data->PageID;

        HTMLview_SetPath(obj, url, data);

        DoMethod(obj, MUIM_HTMLview_StartParser);
        SetAttrs(obj, MUIA_HTMLview_Title, NULL, TAG_DONE);
      }
      delete tmpstr;
    }
    break;

    case MUIM_HTMLview_Post:
    {
      ENTER();

      struct MUIP_HTMLview_Post *pmsg = (struct MUIP_HTMLview_Post *)msg;
      data->PostData = pmsg->Data;
      data->EncodingType = pmsg->EncodingType;

      STRPTR url = new (std::nothrow) char[strlen(pmsg->URL)+20];
      if (!url) return 0;
      sprintf(url, "%s?{%lu}�", pmsg->URL, data->PageID);

      SetAttrs(data->Share->Obj,
          MUIA_HTMLview_ClickedURL, (ULONG)pmsg->URL,
          MUIA_HTMLview_Target, 	(ULONG)FindTarget(obj, pmsg->Target, data),
          MUIA_HTMLview_Qualifier,  0L,
          TAG_DONE);

      DoMethod(obj, MUIM_HTMLview_GotoURL, (ULONG)pmsg->URL, (ULONG)pmsg->Target);

      data->PostData = NULL;
    }
    break;

    case MUIM_HTMLview_StartParser:
    {
      ENTER();

      STRPTR url = data->URLBase;
      STRPTR page = data->Page, comp_url = new (std::nothrow) char[2 + strlen(url) + strlen(page)];
      if (!comp_url) return 0;
      sprintf(comp_url, "%s%s", url, page);
      struct ParseThreadArgs *args = new (std::nothrow) ParseThreadArgs(data->PageID, obj, data, data->Flags, comp_url, data->PostData);
      delete comp_url;
      if (!args) return 0;

      /*char str_args[10];
      sprintf(str_args, "%lx", (ULONG)args);*/

      data->ParseThread = CreateNewProcTags(
        NP_Entry,     (ULONG)ENTRY(ParseThread),
        NP_Name,      (ULONG)data->ParseThreadName,
        #if !defined(__MORPHOS__)
        NP_StackSize, 512*1024,
        #if defined(__amigaos4__)
        NP_Child,     TRUE,
        #endif
        #else
        NP_CodeType,     CODETYPE_PPC,
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
        TAG_DONE);

      if (data->ParseThread)
      {
        struct MsgPort replyPort;
        struct ParseThreadStartupMessage startup;

        memset(&replyPort, 0, sizeof(replyPort));
        replyPort.mp_Node.ln_Type = NT_MSGPORT;
        NewList(&replyPort.mp_MsgList);
        replyPort.mp_SigBit = SIGB_SINGLE;
        replyPort.mp_SigTask = FindTask(NULL);

        memset(&startup, 0, sizeof(startup));
        startup.message.mn_ReplyPort = &replyPort;
        startup.args = args;

        PutMsg(&data->ParseThread->pr_MsgPort, (struct Message *)&startup);
        Remove((struct Node *)WaitPort(&replyPort));

        data->ParseCount++;
      }
      else
        delete args;

      //kprintf("Create Proc OBJ:%lx Thread:%lx ARGS:%lx\n",obj,data->ParseThread,args);
	}
    break;

    case MUIM_HTMLview_Parsed:
    break;

    case MUIM_HTMLview_Period:
    {
      ENTER();

      struct ParseMessage *pmsg;
      if((pmsg = data->PMsg))
      {
        ULONG n_parsed = pmsg->Parsed + (pmsg->Current-pmsg->Buffer);
        if(n_parsed || (data->Flags & FLG_RemoveChildren))
        {
          ULONG parsed = n_parsed - data->Parsed;
          DoMethod(obj, MUIM_HTMLview_Parsed, n_parsed);

          /* Layout and draw the new elements */
          if(parsed || data->PMsg->Gadgets || (data->Flags & FLG_RemoveChildren))
          {
            data->Flags |= FLG_NotResized;
            if(DoMethod(obj, MUIM_Group_InitChange))
            {
              if(data->Flags & FLG_RemoveChildren)
                DoMethod(obj, MUIM_HTMLview_RemoveChildren);

              if(data->PMsg->Gadgets)
              {
                data->PMsg->Gadgets = FALSE;
                struct AppendGadgetMessage amsg(obj, data);
                data->HostObject->AppendGadget(amsg);
              }

              DoMethod(obj, MUIM_Group_ExitChange);

              class AClass *anchor = data->Local ? data->HostObject->FindAnchor(data->Local) : NULL;
              if(anchor)
              {
                delete data->Local;
                data->Local = NULL;
                SetAttrs(obj, MUIA_Virtgroup_Top, (anchor->top() > 5) ? anchor->top()-5 : 0, TAG_DONE);
              }
            }
            data->Flags &= ~FLG_NotResized;
          }

          if(data->PMsg->Title)
          {
            SetAttrs(obj, MUIA_HTMLview_Title, (ULONG)data->PMsg->Title, TAG_DONE);
            data->PMsg->Title = NULL;
          }
        }
        data->Parsed = result = n_parsed;
      }
    }
    break;

    case MUIM_HTMLview_ExtMessage:
    {
      ENTER();
	  ULONG msgs = 0;

      struct ParseInfoMsg *msg;
      while((msg = (struct ParseInfoMsg *)GetMsg(&data->MessagePort)))
      {
        msgs++;

        switch(msg->Class)
        {
          case ParseMsg_Startup:
          {
            if(data->PageID == msg->Unique)
            {
              data->PMsg = msg->Startup.PMsg;
              data->VirtHeight = 0;
              if(data->Flags & FLG_HostObjNotUsed)
              {
                delete data->HostObject;
                data->Flags &= ~FLG_HostObjNotUsed;
              }

              data->HostObject = NULL;
              data->Left = data->Top = 0;
              SetAttrs(obj, MUIA_Virtgroup_Height, 0, TAG_DONE);
              SetAttrs(obj,
                MUIA_Virtgroup_Left,  0,
                MUIA_Virtgroup_Top, 0,
                TAG_DONE);

              data->LayoutMsg.Reset(data->Width, data->Height);
              data->HostObject = msg->Startup.Object;

              data->Period.Start(obj);
              data->Parsed = 0;
              data->Flags |= FLG_RemoveChildren;
              DoMethod(obj, MUIM_HTMLview_Period);
              data->Flags &= ~FLG_RemoveChildren;

              Object *parent;
              if((parent = (Object *)xget(obj, MUIA_Parent)))
                SetAttrs(parent, MUIA_ScrollGroup_Frames, FALSE, TAG_DONE);
            }
          }
          break;

          case ParseMsg_Shutdown:
          {
            if(data->PageID == msg->Unique)
            {
              /*if(!data->PMsg)
              {
                DisplayBeep(NULL);
                kprintf(">>>>>>>>>>>>>>>>>>>>>>>>>>>0x%lx - 0x%lx - (0x%lx == 0x%lx)\n", data->ParseThread, data->Flags, data->HostObject, msg->Shutdown.Object);
                //Wait(4096);
                data->ParseCount++;
              }
              else*/

    		  {
                //ULONG parsed = data->PMsg->Parsed + (data->PMsg->Current-data->PMsg->Buffer);
                //if(parsed != data->Parsed)
                  DoMethod(obj, MUIM_HTMLview_Period);

                struct GetImagesMessage gmsg(obj);
                data->HostObject->GetImages(gmsg);
                if(data->Flags & FLG_Shown)
                {
                  data->Images = gmsg.Images;
                  DoMethod(obj, MUIM_HTMLview_LoadImages, (ULONG)gmsg.Images);
                }

                data->PMsg = NULL;
                data->ParseThread = NULL;

                data->Flags |= FLG_HostObjNotUsed;
                data->Period.Stop(obj);

                if(!data->HostObject->Body)
                  MUI_Redraw(obj, MADF_DRAWOBJECT);
                else if(data->HostObject->Body->id() == tag_FRAMESET)
                {
                  Object *parent;
                  if((parent = (Object *)xget(obj, MUIA_Parent)))
                    SetAttrs(parent, MUIA_ScrollGroup_Frames, TRUE, TAG_DONE);
                }

                if(data->HostObject->RefreshURL)
                  DoMethod(obj, MUIM_HTMLview_StartRefreshTimer);
              }
//              data->ParseCount--;
        	}
            else
            {
              if(data->HostObject != msg->Shutdown.Object)
                delete msg->Shutdown.Object;
            }
              data->ParseCount--;
              //kprintf("ParseMessage %lx %ld\n",obj,data->ParseCount);
          }
          break;

          case ParseMsg_Abort:
          {
//            data->ParseCount--;
            //kprintf("ParseMessage %lx %ld\n",obj,data->ParseCount);
            if(data->PageID == msg->Unique)
            {
	            data->ParseCount--;
              delete data->URLBase;
              delete data->Page;
              delete data->Local;
              data->URLBase = data->Page = data->Local = NULL;

              data->ParseThread = NULL;
            }
          }
          break;
        }
        ReplyMsg(&msg->nm_node);
      }

      result = msgs;
	}
    break;

    case MUIM_HTMLview_LoadImages:
    {
      ENTER();

      struct MUIP_HTMLview_LoadImages *lmsg = (struct MUIP_HTMLview_LoadImages *)msg;

      struct ImageList *first = lmsg->Images;
      while(first && (data->RunningDecoderThreads++ < IMAGE_QUEUE_SIZE || !(data->Flags & FLG_RootObj)))
      {
        DecodeImage(obj, cl, first, data);
        first = first->Next;
      }

      data->ImagesLeft = first;

      if(lmsg->Images)
      {
        data->Share->ImageTimer.Start(obj);
        DoMethod(data->Share->Obj, MUIM_HTMLview_DrawImages);
      }
    }
    break;

    case MUIM_HTMLview_DrawImages:
    {
      ENTER();

      ULONG total = data->Share->DecodeQueue.DumpQueue();
      data->RunningDecoderThreads = total;

      if(total < IMAGE_QUEUE_SIZE && data->ImagesLeft)
      {
        DoMethod(obj, MUIM_HTMLview_LoadImages, (ULONG)data->ImagesLeft);
        total = data->Share->DecodeQueue.DumpQueue();
        data->RunningDecoderThreads = total;
      }

      if(!total)
        data->Share->ImageTimer.Stop(obj);

      SetAttrs(obj, MUIA_HTMLview_ImagesInDecodeQueue, total, TAG_DONE);
    }
    break;

    case MUIM_HTMLview_AddPart:
    {
      ENTER();
      result = (ULONG)HTMLview_AddPart(obj, (struct MUIP_HTMLview_AddPart *)msg, data);
    }
    break;

    case MUIM_HTMLview_VLink:
      ENTER();
    break;

    case MUIM_HTMLview_LookupFrame:
    {
      ENTER();
      struct MUIP_HTMLview_LookupFrame *lmsg = (struct MUIP_HTMLview_LookupFrame *)msg;
      if(data->HostObject)
        result = (ULONG)data->HostObject->LookupFrame(lmsg->Name, data->HostObject);
    }
    break;

    case MUIM_HTMLview_Search:
    {
      ENTER();

      struct MUIP_HTMLview_Search *fmsg = (struct MUIP_HTMLview_Search *)msg;

      ULONG top = fmsg->Flags & MUIF_HTMLview_Search_FromTop ? 0 : data->Top;

      struct FindMessage *find;
      if(data->HostObject && (find = data->HostObject->Find(fmsg->String, top, fmsg->Flags)))
      {
        if(find != (struct FindMessage *)TRUE)
          SetAttrs(obj, MUIA_Virtgroup_Top, find->TopPos > 5 ? find->TopPos - 5 : 0, TAG_DONE);

        result = TRUE;
      }
    }
    break;

    case MUIM_HTMLview_StartRefreshTimer:
    {
      ENTER();
      data->RefreshTimer.Stop(obj);
      data->RefreshTimer.ihn.ihn_Object = obj;
      data->RefreshTimer.ihn.ihn_Method = MUIM_HTMLview_Refresh;
      data->RefreshTimer.ihn.ihn_Flags  = MUIIHNF_TIMER;
      data->RefreshTimer.ihn.ihn_Millis = data->HostObject->RefreshTime;
      data->RefreshTimer.ihn.ihn_Current = 0;
      data->RefreshTimer.Start(obj);
//      kprintf("Start refresh timer (%ld, %ld)\n", data->HostObject->RefreshTime, data->RefreshTimer.ihn_Current);
    }
    break;

    case MUIM_HTMLview_Refresh:
    {
      ENTER();
      data->RefreshTimer.Stop(obj);
      STRPTR url;
      if((url = (STRPTR)DoMethod(obj, MUIM_HTMLview_AddPart, (ULONG)data->HostObject->RefreshURL)))
      {
        SetAttrs(data->Share->Obj,
          MUIA_HTMLview_ClickedURL, (ULONG)url,
          MUIA_HTMLview_Target, 	(ULONG)data->FrameName,
          MUIA_HTMLview_Qualifier,  NULL,
          TAG_DONE);
        delete url;
      }
    }
    break;

    case MUIM_HTMLview_HitTest:
    {
      ENTER();
      struct MUIP_HTMLview_HitTest *hmsg = (struct MUIP_HTMLview_HitTest *)msg;
      class HostClass *host;
      if((host = data->HostObject))
      {
        hmsg->HMsg->X += data->Left;
        hmsg->HMsg->Y += data->Top;
        result = (ULONG)host->HitTest(*(hmsg->HMsg));
        hmsg->HMsg->X -= data->Left;
        hmsg->HMsg->Y -= data->Top;
      }
    }
    break;

    case MUIM_HTMLview_GetContextInfo:
    {
      ENTER();
      struct MUIP_HTMLview_GetContextInfo *cmsg = (struct MUIP_HTMLview_GetContextInfo *)msg;

      class HostClass *host;
      if((host = data->HostObject))
      {
        LONG x = cmsg->X-_mleft(obj), y = cmsg->Y-_mtop(obj);
        struct HitTestMessage *hmsg = new (std::nothrow) struct HitTestMessage(x + data->Left, y + data->Top, host);
        if (!hmsg) return 0;
        if(!host->HitTest(*hmsg))
          hmsg->URL = NULL;

        struct MUIR_HTMLview_GetContextInfo *cinfo = &data->ContextInfo;
        STRPTR src[] = { hmsg->URL, hmsg->ImgSrc, hmsg->Frame, hmsg->Background };
        STRPTR *dst[] = { &cinfo->URL, &cinfo->Img, &cinfo->Frame, &cinfo->Background };
        Object *frame = (cinfo->FrameObj = hmsg->FrameObj) ? hmsg->FrameObj : obj;
        cinfo->Target = hmsg->Target;
        for(UWORD i = 0; i < 4; i++)
        {
          delete *dst[i];
          *dst[i] = src[i] ? (STRPTR)DoMethod(frame, MUIM_HTMLview_AddPart, (ULONG)src[i]) : (STRPTR)NULL;
        }

        if(hmsg->Img)
        {
          struct PictureFrame *pic;
          if((pic = hmsg->Img->Picture))
              cinfo->ImageSize = pic->Size();
          else  cinfo->ImageSize = 0;

          cinfo->ImageWidth = hmsg->Img->Width();
          cinfo->ImageHeight = hmsg->Img->Height();
          cinfo->ImageOffsetX = hmsg->OffsetX;
          cinfo->ImageOffsetY = hmsg->OffsetY;
          cinfo->ImageAltText = hmsg->Img->AltText;
        }

        delete hmsg;
        result = (ULONG)cinfo;
      }
    }
    break;

    case MUIM_HTMLview_AddSingleAnim:
    {
      ENTER();
      struct MUIP_HTMLview_AddSingleAnim *amsg = (struct MUIP_HTMLview_AddSingleAnim *)msg;
      struct ObjectList *receivers = new (std::nothrow) struct ObjectList(amsg->receiver);
      if (!receivers) return 0;
      struct AnimInfo *item;
      if((item = data->Share->AddAnim(obj, data, amsg->picture, receivers)))
        item->Flags |= AnimFLG_DeleteObjList;
    }
    break;

    case MUIM_HTMLview_AnimTick:
    {
      ENTER();
      data->Share->AnimTimer.ihn.ihn_Millis = data->Share->Anims->Tick();
    }
    break;

    case MUIM_HTMLview_FlushImage:
    {
      ENTER();
      struct MUIP_HTMLview_FlushImage *fmsg = (struct MUIP_HTMLview_FlushImage *)msg;

      data->Share->ImageStorage->FlushCache(fmsg->URL);

      LONG magic_value = (LONG)fmsg->URL;
      if(magic_value == MUIV_HTMLview_FlushImage_Nondisplayed)
        break;

      struct ImageList *images = data->Images;
      LONG top = data->Top, bottom = data->Top+data->Height-1;
      BOOL flush = magic_value == MUIV_HTMLview_FlushImage_Displayed || magic_value == MUIV_HTMLview_FlushImage_All;
      while(images)
      {
        if(flush || !strcmp(images->ImageName, fmsg->URL))
        {
          struct ObjectList *first = images->Objects;
          while(first)
          {
            if(first->Obj->FlushImage(top, bottom))
            {
              data->RedrawObj = first->Obj;
              MUI_Redraw(obj, MADF_DRAWUPDATE);
            }
            first = first->Next;
          }
        }
        images = images->Next;
      }
    }
    break;

    case MUIM_HTMLview_ServerRequest:
    {
      ENTER();
      struct MUIP_HTMLview_ServerRequest *smsg = (struct MUIP_HTMLview_ServerRequest *)msg;
      STRPTR dummy, url = new (std::nothrow) char[strlen(data->URL) + strlen(smsg->Argument) + 2];
      if (!url) return 0;
      if((dummy = strstr(strcpy(url, data->URL), "?")))
        *dummy = '\0';
      sprintf(url+strlen(url), "?%s", smsg->Argument);
      SetAttrs(data->Share->Obj,
        MUIA_HTMLview_ClickedURL, (ULONG)url,
        MUIA_HTMLview_Target,     (ULONG)data->FrameName,
        MUIA_HTMLview_Qualifier,  NULL,
        TAG_DONE);
      delete[] url;
    }
    break;

    case MUIM_HTMLview_PauseAnims:
    {
      ENTER();
      data->Share->AnimTimer.Stop(obj);
    }
    break;

    case MUIM_HTMLview_ContinueAnims:
    {
      ENTER();
      if(data->Share->Anims)
        data->Share->AnimTimer.Start(obj);
    }
    break;

    default:
    {
      ENTER();
      result = DoSuperMethodA(cl, obj, (Msg)msg);
    }
    break;
  }

  RETURN(result);
  return(result);
}
