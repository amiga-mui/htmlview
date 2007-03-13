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

#include <stdio.h>
#include <clib/alib_protos.h>
#include <clib/macros.h>
#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "General.h"
#include "ScrollGroup.h"
#include "private.h"
#include "HTMLview_mcp.h"

struct MUI_CustomClass *ScrollGroupClass;

VOID Layout (ULONG width, ULONG height, struct ScrollGroupData *data, struct MUI_LayoutMsg *lmsg = NULL)
{
	ULONG left = 0, top = 0, knobheight = 0;
	ULONG scrollwidth = _minwidth(data->RightScroll), scrollheight = _minheight(data->BottomScroll);
	BOOL leftscr = FALSE, topscr = FALSE;

	ULONG rightheight = _minheight(data->RightScroll);
	if((data->Flags & FLG_RightVisible) && width > scrollwidth && height-scrollheight >= rightheight)
	{
		if(data->Flags & FLG_BottomVisible)
			knobheight = scrollheight;

		rightheight = height - knobheight;
		width -= scrollwidth;
		leftscr = TRUE;
	}

	ULONG bottomwidth = _minwidth(data->BottomScroll);
	if((data->Flags & FLG_BottomVisible) && height > scrollheight && width >= bottomwidth)
	{
		bottomwidth = width;
		height -= scrollheight;
		topscr = TRUE;
	}

	if(!height)	height = 1;
	if(!width)	width = 1;

	MUI_Layout(data->HTMLview, left, top, width, height, 0L);
	MUI_Layout(data->RightScroll, left+width, top, scrollwidth, rightheight, 0L);
	MUI_Layout(data->BottomScroll, left, top+height, bottomwidth, scrollheight, 0L);
	MUI_Layout(data->Knob, left+width, top+height, scrollwidth, scrollheight, 0L);

	if(topscr && leftscr)
			data->Flags |= FLG_KnobVisible;
	else	data->Flags &= ~FLG_KnobVisible;

	if(lmsg)
	{
		lmsg->lm_Layout.Width = width + scrollwidth;
		lmsg->lm_Layout.Height = height + scrollheight;
	}
}

HOOKPROTO(ScrollGroupLayoutCode, ULONG, Object *obj, struct MUI_LayoutMsg *lmsg)
{
	switch(lmsg->lm_Type)
	{
		case MUILM_MINMAX:
		{
			lmsg->lm_MinMax.MinWidth	= 1;
			lmsg->lm_MinMax.MinHeight	= 1;
			lmsg->lm_MinMax.MaxWidth	= MUI_MAXMAX;
			lmsg->lm_MinMax.MaxHeight	= MUI_MAXMAX;
			lmsg->lm_MinMax.DefWidth	= 100;
			lmsg->lm_MinMax.DefHeight	= 100;
		}
		return(0);

		case MUILM_LAYOUT:
		{
			struct ScrollGroupData *data = (struct ScrollGroupData *)INST_DATA(OCLASS(obj), obj);
			Layout(lmsg->lm_Layout.Width, lmsg->lm_Layout.Height, data, lmsg);
		}
		return(TRUE);
	}
}
MakeHook(ScrollGroupLayoutHook, ScrollGroupLayoutCode);

#define GetConfigVal(o, i, v) GetConfigItemA(o, i, (ULONG)v, TRUE)
ULONG GetConfigItemA (Object *obj, ULONG item, ULONG def_value, BOOL de_ref = FALSE);

DISPATCHERPROTO(ScrollGroupDispatcher)
{
	ULONG result = NULL;
	struct ScrollGroupData *data = (struct ScrollGroupData *)INST_DATA(cl, obj);

	switch(msg->MethodID)
	{
		case OM_NEW:
		{
			struct opSet *nmsg = (struct opSet *)msg;
			Object *HTMLview, *Knob, *RightScroll, *BottomScroll;
			struct TagItem *tag;

      ENTER();

			if(tag = FindTagItem(MUIA_ScrollGroup_HTMLview, nmsg->ops_AttrList))
			{
				HTMLview = (Object *)tag->ti_Data;
				tag->ti_Tag = Child;
			}

			RightScroll = ScrollbarObject, End;
			BottomScroll = ScrollbarObject, MUIA_Group_Horiz, TRUE, End;
			Knob = RectangleObject, ButtonFrame, MUIA_Background, MUII_ButtonBack, End;

			struct TagItem tags[] =
			{
				MUIA_Group_LayoutHook, (ULONG)&ScrollGroupLayoutHook,
				MUIA_Virtgroup_Input, FALSE,
				Child, (ULONG)RightScroll,
				Child, (ULONG)BottomScroll,
				Child, (ULONG)Knob,
				TAG_MORE, (ULONG)nmsg->ops_AttrList
			};

			if(obj = (Object *)DoSuperMethod(cl, obj, OM_NEW, tags, nmsg->ops_GInfo))
			{
				struct ScrollGroupData *data = (struct ScrollGroupData *)INST_DATA(cl, obj);

				data->HTMLview = HTMLview;
				data->Knob = Knob;
				data->RightScroll = RightScroll;
				data->BottomScroll = BottomScroll;

				DoMethod(HTMLview, MUIM_Notify, MUIA_Virtgroup_Left,				MUIV_EveryTime, BottomScroll, 3, MUIM_Set, MUIA_Prop_First,			MUIV_TriggerValue);
				DoMethod(HTMLview, MUIM_Notify, MUIA_Width,							MUIV_EveryTime, BottomScroll, 3, MUIM_Set, MUIA_Prop_Visible,		MUIV_TriggerValue);
				DoMethod(HTMLview, MUIM_Notify, MUIA_Virtgroup_Top,				MUIV_EveryTime, RightScroll,	3, MUIM_Set, MUIA_Prop_First,			MUIV_TriggerValue);
				DoMethod(HTMLview, MUIM_Notify, MUIA_Height,							MUIV_EveryTime, RightScroll,	3, MUIM_Set, MUIA_Prop_Visible,		MUIV_TriggerValue);
				DoMethod(HTMLview, MUIM_Notify, MUIA_HTMLview_Prop_VDeltaFactor, MUIV_EveryTime, RightScroll,  3, MUIM_Set, MUIA_Prop_DeltaFactor, MUIV_TriggerValue);
				DoMethod(HTMLview, MUIM_Notify, MUIA_HTMLview_Prop_HDeltaFactor, MUIV_EveryTime, BottomScroll, 3, MUIM_Set, MUIA_Prop_DeltaFactor, MUIV_TriggerValue);
				DoMethod(RightScroll,	MUIM_Notify, MUIA_Prop_First,				MUIV_EveryTime, HTMLview,		3, MUIM_Set, MUIA_Virtgroup_Top,		MUIV_TriggerValue);
				DoMethod(BottomScroll,	MUIM_Notify, MUIA_Prop_First,				MUIV_EveryTime, HTMLview,		3, MUIM_Set, MUIA_Virtgroup_Left,	MUIV_TriggerValue);

				if((tag = FindTagItem(MUIA_ScrollGroup_Smooth, nmsg->ops_AttrList)) && tag->ti_Data)
					data->Flags |= FLG_SmoothScroll;

				if(tag = FindTagItem(MUIA_ScrollGroup_Scrolling, nmsg->ops_AttrList))
				{
					switch(tag->ti_Data)
					{
						case MUIV_HTMLview_Scrollbars_Yes:
							data->Flags |= FLG_RightVisible | FLG_BottomVisible;
						case MUIV_HTMLview_Scrollbars_No:
							DoMethod(HTMLview, MUIM_Notify, MUIA_Virtgroup_Width,		MUIV_EveryTime, BottomScroll, 3, MUIM_Set, MUIA_Prop_Entries, MUIV_TriggerValue);
							DoMethod(HTMLview, MUIM_Notify, MUIA_Virtgroup_Height,	MUIV_EveryTime, RightScroll, 3, MUIM_Set, MUIA_Prop_Entries, MUIV_TriggerValue);
						break;

						case MUIV_HTMLview_Scrollbars_HorizAuto:
							data->Flags |= FLG_RightVisible | FLG_HorizAuto;
						case MUIV_HTMLview_Scrollbars_Auto:
							DoMethod(HTMLview, MUIM_Notify, MUIA_Virtgroup_Width,		MUIV_EveryTime, BottomScroll, 3, MUIM_Set, MUIA_Prop_Entries, MUIV_TriggerValue);
							DoMethod(HTMLview, MUIM_Notify, MUIA_Virtgroup_Height,	MUIV_EveryTime, RightScroll, 3, MUIM_Set, MUIA_Prop_Entries, MUIV_TriggerValue);

//							DoMethod(HTMLview, MUIM_Notify, MUIA_Virtgroup_Width,		MUIV_EveryTime, obj, 2, MUIM_ScrollGroup_NewWidth, MUIV_TriggerValue);
							DoMethod(HTMLview, MUIM_Notify, MUIA_Virtgroup_Width,		MUIV_EveryTime, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, obj, 1, MUIM_ScrollGroup_NewWidth); //, MUIV_TriggerValue);
//							DoMethod(HTMLview, MUIM_Notify, MUIA_Virtgroup_Height,	MUIV_EveryTime, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, obj, 2, MUIM_ScrollGroup_NewHeight, MUIV_TriggerValue);
						break;
					}
				}

        RETURN(obj);
				return((ULONG)obj);
			}
//			CoerceMethod(cl, obj, OM_DISPOSE);
		}
		break;

		case MUIM_Backfill:
		{
      ENTER();
		}
		break;

		case OM_SET:
		{
			struct opSet *smsg = (struct opSet *)msg;
			struct TagItem *tag;

      ENTER();

			if(tag = FindTagItem(MUIA_ScrollGroup_Frames, smsg->ops_AttrList))
			{
				if(data->Flags & FLG_HorizAuto)
				{
					ULONG flags = data->Flags;
					if(!tag->ti_Data)
							flags |= FLG_RightVisible;
					else	flags &= ~FLG_RightVisible;

					if(flags != data->Flags)
					{
						data->Flags = flags;
						if(DoMethod(obj, MUIM_Group_InitChange))
							DoMethod(obj, MUIM_Group_ExitChange);
					}
				}
			}
			else
			{
				result = DoSuperMethodA(cl, obj, msg);
			}
		}
		break;

		case OM_GET:
		{
			struct opGet *gmsg = (struct opGet *)msg;

      ENTER();

			if(gmsg->opg_AttrID == MUIA_ScrollGroup_HTMLview)
			  *gmsg->opg_Storage = (ULONG)data->HTMLview, result = TRUE;
			else
        result = DoSuperMethodA(cl, obj, msg);
		}
		break;

		case MUIM_Setup:
		{
      ENTER();

			if(result = DoSuperMethodA(cl, obj, msg))
			{
				if(GetConfigVal(data->HTMLview, MUICFG_HTMLview_PageScrollSmooth, TRUE))
				{
					SetAttrs(data->RightScroll,	 0x804236ce, TRUE, TAG_DONE);
					SetAttrs(data->BottomScroll, 0x804236ce, TRUE, TAG_DONE);
				}

				data->Events.ehn_Priority	= 0;
				data->Events.ehn_Flags		= 0;
				data->Events.ehn_Object		= obj;
				data->Events.ehn_Class  	= cl;
				data->Events.ehn_Events		= IDCMP_MOUSEBUTTONS;
				DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->Events);
			}
		}
		break;

		case MUIM_Cleanup:
		{
      ENTER();

			DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->Events);
			result = DoSuperMethodA(cl, obj, msg);
		}
		break;

		case MUIM_HandleEvent:
		{
			struct MUIP_HandleEvent *hmsg = (struct MUIP_HandleEvent *)msg;
			struct IntuiMessage *imsg = hmsg->imsg;
			
      ENTER();

      if(imsg && data->Flags & FLG_KnobVisible)
			{
				switch(imsg->Class)
				{
					case IDCMP_MOUSEBUTTONS:
					{
						Object *knob = data->Knob;
						LONG	x = imsg->MouseX - _left(knob),
								y = imsg->MouseY - _top(knob);

						if(imsg->Code == IECODE_LBUTTON)
						{
							if(x > 0 && y > 0 && x < _width(knob) && y < _height(knob))
							{
								SetAttrs(knob, MUIA_Selected, TRUE, TAG_DONE);
								data->KnobPressX = imsg->MouseX;
								data->KnobPressY = imsg->MouseY;
								data->VScrollValue = xget(data->RightScroll, MUIA_Prop_First);
								data->HScrollValue = xget(data->BottomScroll, MUIA_Prop_First);

								DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->Events);
								data->Events.ehn_Events |= IDCMP_MOUSEMOVE;
								DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->Events);
							}
						}
						else if(imsg->Code == IECODE_LBUTTON + IECODE_UP_PREFIX)
						{
							SetAttrs(knob, MUIA_Selected, FALSE, TAG_DONE);
							DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->Events);
							data->Events.ehn_Events &= ~IDCMP_MOUSEMOVE;
							DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->Events);
						}
					}
					break;

					case IDCMP_MOUSEMOVE:
					{
						LONG x = imsg->MouseX - data->KnobPressX;
						LONG y = imsg->MouseY - data->KnobPressY;

						LONG v_scroll = data->VScrollValue + 45*y / 10;
						LONG h_scroll = data->HScrollValue + 45*x / 10;

						LONG v_visible = xget(data->RightScroll, MUIA_Prop_Visible);
						LONG v_entries = xget(data->RightScroll, MUIA_Prop_Entries);
						LONG h_visible = xget(data->BottomScroll, MUIA_Prop_Visible);
						LONG h_entries = xget(data->BottomScroll, MUIA_Prop_Entries);
						LONG v_max = v_entries-v_visible;
					  LONG h_max = h_entries-h_visible;

						SetAttrs(data->RightScroll, MUIA_Prop_First, MAX(0, MIN(v_max, v_scroll)), TAG_DONE);
						SetAttrs(data->BottomScroll, MUIA_Prop_First, MAX(0, MIN(h_max, h_scroll)), TAG_DONE);
					}
					break;
				}
			}
		}
		break;

		case MUIM_ScrollGroup_NewWidth:
		{
			ULONG width; // = ((ULONG *)msg)[1];

      ENTER();

			width = xget(data->HTMLview, MUIA_Virtgroup_Width);
//			set(data->BottomScroll, MUIA_Prop_Entries, width);

			ULONG height = xget(data->HTMLview, MUIA_Virtgroup_Height);
//			set(data->RightScroll, MUIA_Prop_Entries, height);

			ULONG flags = data->Flags;
			if(_width(data->HTMLview) < width)
					flags |= FLG_BottomVisible;
			else	flags &= ~FLG_BottomVisible;

			if(!(data->Flags & FLG_HorizAuto))
			{
				if(_height(data->HTMLview) < height)
						flags |= FLG_RightVisible;
				else	flags &= ~FLG_RightVisible;
			}

			if(flags != data->Flags)
			{
				data->Flags = flags;
				if(DoMethod(obj, MUIM_Group_InitChange))
					DoMethod(obj, MUIM_Group_ExitChange);
			}
		}
		break;

		case MUIM_AskMinMax:
		{
			struct MUIP_AskMinMax *amsg = (struct MUIP_AskMinMax *)msg;

      ENTER();

			result = DoSuperMethodA(cl, obj, msg);

			amsg->MinMaxInfo->MinWidth  = _subwidth(obj)  + _minwidth(data->HTMLview);
			amsg->MinMaxInfo->MinHeight = _subheight(obj) + _minheight(data->HTMLview);
			amsg->MinMaxInfo->DefWidth  = 200;
			amsg->MinMaxInfo->DefHeight = 200;
			amsg->MinMaxInfo->MaxWidth  = MBQ_MUI_MAXMAX;
			amsg->MinMaxInfo->MaxHeight = MBQ_MUI_MAXMAX;
		}
		break;

		default:
		{
      ENTER();
			result = DoSuperMethodA(cl, obj, msg);
		}
		break;
	}

  RETURN(result);
	return(result);
}
