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

#include <stdlib.h>
#include <clib/alib_protos.h>
#include <libraries/mui.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "General.h"
#include "ParseMessage.h"
#include "SharedData.h"
#include "private.h"

#include "classes/HostClass.h"

#include "rev.h"

BOOL mSet (Object *obj, struct IClass *cl, struct opSet *msg)
{
	struct HTMLviewData *data = (struct HTMLviewData *)INST_DATA(cl, obj);
	BOOL forward = FALSE;
	struct TagItem *tag, *tags = msg->ops_AttrList;

  ENTER();

	while(tag = NextTagItem(&tags))
	{
		LONG ti_Data = tag->ti_Data;
		switch(tag->ti_Tag)
		{
/*			case MUIA_Virtgroup_Left:
			{
				LONG left = data->Left, diff;
				data->Left = (data->VirtWidth > data->Width) ? min(data->VirtWidth-data->Width, max(0, ti_Data)) : 0;
				if((diff = data->Left - left) && abs(diff) < data->Height)
					data->HScrollDiff = diff;
			}
			break;

			case MUIA_Virtgroup_Top:
			{
				LONG top = data->Top, diff;
				data->Top = (data->VirtHeight > data->Height) ? min(data->VirtHeight-data->Height, max(0, ti_Data)) : 0;
				if(diff = data->Top - top)
				{
					if(data->PMsg)
						data->TopChange = 0;
					else if(abs(diff) < data->Height)
						data->VScrollDiff = diff;
				}
				tag->ti_Data = data->Top;
			}
			break;
*/
			case MUIA_HTMLview_Contents:
			{
				ULONG length = strlen((STRPTR)ti_Data);
				struct BufferParseMessage pmsg((STRPTR)ti_Data, length);
				pmsg.HTMLview = obj;

				class HostClass *hobj, *old_object = data->HostObject;

				hobj = new class HostClass(obj, data);
				pmsg.Host = hobj;
				hobj->Parse(pmsg);
				hobj->setFlags(hobj->flags() | FLG_AllElementsPresent);

				data->LayoutMsg.Reset(data->Width, data->Height);
				data->HostObject = hobj;

				DoMethod(obj, MUIM_HTMLview_AbortAll);

				if(DoMethod(obj, MUIM_Group_InitChange))
				{
					DoMethod(obj, MUIM_HTMLview_RemoveChildren);
					if(pmsg.Gadgets)
					{
						struct AppendGadgetMessage amsg(obj, data);
						data->HostObject->AppendGadget(amsg);
					}
					DoMethod(obj, MUIM_Group_ExitChange);
					set(obj, MUIA_HTMLview_Title, pmsg.Title);

					if(data->Flags & FLG_Shown)
					{
						struct GetImagesMessage gmsg(obj);
						data->HostObject->GetImages(gmsg);
						data->Images = gmsg.Images;
						DoMethod(obj, MUIM_HTMLview_LoadImages, gmsg.Images);
					}
				}
//				else kprintf("*** ERROR: InitChange\n");

				if(data->Flags & FLG_HostObjNotUsed)
					delete old_object;

				if(data->HostObject->RefreshURL)
					DoMethod(obj, MUIM_HTMLview_StartRefreshTimer);

				data->Flags |= FLG_HostObjNotUsed;
			}
			break;

			case MUIA_HTMLview_Target:
				data->Target = (STRPTR)ti_Data;
			break;

			case MUIA_Group_Forward:
				forward = (BOOL)ti_Data;
			break;

			case MUIA_HTMLview_LoadHook:
				data->LoadHook = (struct Hook *)ti_Data;
			break;

			case MUIA_HTMLview_ImageLoadHook:
				data->ImageLoadHook = (struct Hook *)ti_Data;
			break;

			case MUIA_HTMLview_FrameName:
				data->FrameName = (STRPTR)ti_Data;
			break;

			case MUIA_HTMLview_MarginWidth:
				data->LayoutMsg.MarginWidth = ti_Data;
			break;

			case MUIA_HTMLview_MarginHeight:
				data->LayoutMsg.MarginHeight = ti_Data;
			break;

			case MUIA_HTMLview_DiscreteInput:
			{
				if(ti_Data)
						data->Flags |= FLG_DiscreteInput;
				else	data->Flags &= ~FLG_DiscreteInput;
			}
			break;

			case MUIA_HTMLview_IntuiTicks:
			{
				DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->Share->Events);
				if(ti_Data)
						data->Share->Events.ehn_Events |= IDCMP_INTUITICKS;
				else	data->Share->Events.ehn_Events &= ~IDCMP_INTUITICKS;
				DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->Share->Events);
			}
			break;

			case MUIA_HTMLview_Title:
				data->Title = (STRPTR)ti_Data;
			break;

			case MUIA_HTMLview_Qualifier:
				data->Qualifier = ti_Data;
			break;

			case MUIA_HTMLview_ImagesInDecodeQueue:
				if(ti_Data == data->Share->ImagesInDecodeQueue)
					tag->ti_Tag = TAG_IGNORE;
				data->Share->ImagesInDecodeQueue = ti_Data;
			break;
		}
	}

  RETURN(forward);
	return(forward);
}

ULONG mGet (Object *obj, struct IClass *cl, struct opGet *msg)
{
	struct HTMLviewData *data = (struct HTMLviewData *)INST_DATA(cl, obj);
	ULONG ti_Data;

  ENTER();

	switch(msg->opg_AttrID)
	{
		case MUIA_Version:
			ti_Data = (ULONG)LIB_VERSION;
		break;

		case MUIA_Revision:
			ti_Data = (ULONG)LIB_REVISION;
		break;

		case MUIA_HTMLview_URL:
			ti_Data = (ULONG)data->URL;
		break;

		case MUIA_HTMLview_Qualifier:
			ti_Data = data->Qualifier;
		break;

		case MUIA_HTMLview_CurrentURL:
			ti_Data = (ULONG)data->LastURL;
		break;

		case MUIA_HTMLview_PageID:
			ti_Data = data->PageID;
		break;

		/* Just so that one can take notify on them... */
		case MUIA_HTMLview_ImagesInDecodeQueue:
		case MUIA_HTMLview_Prop_HDeltaFactor:
		case MUIA_HTMLview_Prop_VDeltaFactor:
		case MUIA_HTMLview_ClickedURL:
			ti_Data = NULL;
		break;

		case MUIA_HTMLview_Title:
			ti_Data = (ULONG)data->Title;
		break;

		case MUIA_HTMLview_Target:
			ti_Data = (ULONG)data->Target;
		break;

		case MUIA_HTMLview_FrameName:
			ti_Data = (ULONG)data->FrameName;
		break;

		case MUIA_HTMLview_InstanceData:
			ti_Data = (ULONG)data;
		break;

/*		case MUIA_Virtgroup_Height:
			ti_Data = data->VirtHeight;
		break;
*/
		default:
			return(DoSuperMethodA(cl, obj, (Msg)msg));
		break;
	}
	*msg->opg_Storage = ti_Data;

  RETURN(TRUE);
	return(TRUE);
}
