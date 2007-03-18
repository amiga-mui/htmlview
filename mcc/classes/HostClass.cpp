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

#include "HostClass.h"
#include "TextClass.h"
#include "BaseClass.h"

#include "private.h"
#include "SharedData.h"

#include <proto/commodities.h>
#include <proto/keymap.h>

#include <ctype.h>
#include <math.h>
#include <stdio.h>

#ifndef PI
#define PI ((double)3.141592653589793)
#endif

HostClass::~HostClass ()
{
	Data->RefreshTimer.Stop(Obj);
	delete RefreshURL;
}

struct FindMessage *HostClass::Find (STRPTR str, ULONG top, ULONG flags)
{
	struct FindMessage *result = NULL;
	if(Body->id() == tag_FRAMESET)
	{
    #warning "FindDefaultFrame?!?!"
		//if(!DefaultFrame)
		//	DefaultFrame = Body->FindDefaultFrame(0);

		if(DefaultFrame)
			result = (struct FindMessage *)DoMethod(DefaultFrame, MUIM_HTMLview_Search, str, flags);
	}
	else
	{
		FindMsg.SearchString = str;
		FindMsg.Flags = flags;
		FindMsg.TopPos = top;

		class TextClass *last;
		if((last = FindMsg.LastMatch))
		{
			MarkMsg.AddBox(last, last->MarkBegin, last->MarkEnd);
			last->MarkBegin = last->MarkEnd = 0;
			if(!(flags & MUIF_HTMLview_Search_Next))
				FindMsg.LastMatch = NULL;
		}

		LONG len = FindMsg.StrLength = strlen(str), step = 0;
		BYTE *map = FindMsg.SearchMap;
		memset(map, len, 256);
		if(flags & MUIF_HTMLview_Search_CaseSensitive)
		{
			FindMsg.StrCmp = strncmp;
			while(*str)
				map[*str++] = step--;
		}
		else
		{
      #warning "FIXME: here should be strnicmp!"
			FindMsg.StrCmp = strncmp;
			while(*str)
			{
				map[tolower(*str)] = step;
				map[toupper(*str++)] = step--;
			}
		}

		if(Body && (FindMsg.LastMatch = Body->Find(FindMsg)))
			result = &FindMsg;

		if(MarkMsg.FirstBox)
		{
			Data->Flags |= FLG_RedrawMarked;
			MUI_Redraw(Obj, MADF_DRAWUPDATE);
			Data->Flags &= ~FLG_RedrawMarked;
		}
	}
	return(result);
}

Object *HostClass::LookupFrame (STRPTR name, UNUSED class HostClass *hclass)
{
	return(Body ? Body->LookupFrame(name, this) : NULL);
}

ULONG HostClass::HandleEvent (Object *obj, UNUSED struct IClass *cl, struct MUIP_HandleEvent *emsg)
{
	ULONG result = 0;
	struct HTMLviewData *data = Data;
	struct IntuiMessage *imsg = emsg->imsg;

	if(Body->id() == tag_FRAMESET)
	{
		if(imsg->Class == IDCMP_RAWKEY)
		{
      #warning "FindDefaultFrame?!?!"
//			if(!DefaultFrame)
//				DefaultFrame = Body->FindDefaultFrame(0);

			if(DefaultFrame)
				DoMethod(DefaultFrame, MUIM_HTMLview_HandleEvent, emsg->imsg, emsg->muikey);
		}
		else
		{
			Object *result;
			if((result = Body->HandleMUIEvent(emsg)))
				DefaultFrame = result;
		}
	}
	else
	{
		if(imsg->Class == IDCMP_RAWKEY)
		{
			if(data->Flags & FLG_DiscreteInput)
			{
				Object *act = (Object *)xget(_win(obj), MUIA_Window_ActiveObject);

//				get(_win(obj), MUIA_Window_DefaultObject, &def);
//				printf("%p, %p, %p\n", obj, act, def);
				
        if(act != data->Share->Obj && act)
					return(0);
			}

			ULONG muikey;
			if(emsg->muikey == MUIKEY_NONE && (imsg->Qualifier & IEQUALIFIER_NUMERICPAD))
					muikey = NumPadMovement(emsg->imsg);
			else	muikey = emsg->muikey;

			ULONG xvalue = data->Left, yvalue = data->Top;
			switch(muikey)
			{
				case MUIKEY_TOP:
					yvalue = 0;
				break;

				case MUIKEY_BOTTOM:
					yvalue = data->VirtHeight;
				break;

				case MUIKEY_UP:
					yvalue -= data->Share->VDeltaFactor;
				break;

				case MUIKEY_DOWN:
					yvalue += data->Share->VDeltaFactor;
				break;

				case MUIKEY_PAGEUP:
					yvalue -= data->Height;
				break;

				case MUIKEY_PAGEDOWN:
					yvalue += data->Height;
				break;

				case MUIKEY_LINESTART:
					xvalue = 0;
				break;

				case MUIKEY_LINEEND:
					xvalue = data->VirtWidth;
				break;

				case MUIKEY_LEFT:
					xvalue -= data->Share->HDeltaFactor;
				break;

				case MUIKEY_RIGHT:
					xvalue += data->Share->HDeltaFactor;
				break;

				case MUIKEY_WORDLEFT:
					xvalue -= data->Width;
				break;

				case MUIKEY_WORDRIGHT:
					xvalue += data->Width;
				break;
			}

			if(xvalue != data->Left || yvalue != data->Top)
			{
				SetAttrs(obj,
					MUIA_Virtgroup_Left, xvalue,
					MUIA_Virtgroup_Top, yvalue,
					TAG_DONE);

				result = MUI_EventHandlerRC_Eat;
			}
			else
			{
				if(data->Height < data->VirtHeight)
				{
					struct InputEvent event;
					event.ie_NextEvent      = NULL;
					event.ie_Class          = IECLASS_RAWKEY;
					event.ie_SubClass       = 0;

					if(MatchKey(imsg, event, data->Share->PageScrollKey))
					{
						ULONG scroll_height = (data->Height*data->Share->PageScrollMove) / 100;
						ULONG bottom = min(data->VirtHeight-data->Height, data->Top + scroll_height);
						ULONG top = data->Top, height = (bottom - top) / 2;
						if(height)
						{
							if(data->Share->Flags & FLG_SmoothPageScroll)
							{
								ULONG add = 0, last;
								struct MsgPort *port = _window(obj)->UserPort;
								struct IntuiMessage *imsg;
                DOUBLE r = PI;

								for(; r > PI/2; r -= (PI / 30))
								{
									if((imsg = (IntuiMessage *)GetMsg(port)))
									{
										if(imsg->Class == IDCMP_RAWKEY && MatchKey(imsg, event, data->Share->PageScrollKey))
										{
											ReplyMsg((Message *)imsg);
											add += scroll_height;
										}
										else PutMsg(port, (Message *)imsg);
									}

									WaitTOF();
									SetAttrs(obj, MUIA_Virtgroup_Top, last = top+(ULONG)(height*(cos(r)+1)), TAG_DONE);
								}

								ULONG next = top+(ULONG)(height*(cos(r)+1));
								ULONG speed = next - last;
								for(ULONG pos = last; pos < next+add; pos += speed)
								{
									if((imsg = (IntuiMessage *)GetMsg(port)))
									{
										if(imsg->Class == IDCMP_RAWKEY && MatchKey(imsg, event, data->Share->PageScrollKey))
										{
											ReplyMsg((Message *)imsg);
											add += scroll_height;
										}
										else PutMsg(port, (Message *)imsg);
									}

									WaitTOF();
									SetAttrs(obj, MUIA_Virtgroup_Top, pos, TAG_DONE);
								}

								top += add;
								bottom += add;

								for(; r >= 0; r -= (PI / 30))
								{
									WaitTOF();
									SetAttrs(obj, MUIA_Virtgroup_Top, top+(ULONG)(height*(cos(r)+1)), TAG_DONE);
								}
							}
							SetAttrs(obj, MUIA_Virtgroup_Top, bottom, TAG_DONE);
						}
						result = MUI_EventHandlerRC_Eat;
					}
				}
			}
		}
		else
		{
			LONG x = imsg->MouseX - _mleft(obj);
			LONG y = imsg->MouseY - _mtop(obj);

			BOOL map = FALSE;
			STRPTR url = NULL, link = NULL, target = NULL;
			class SuperClass *linkobj;
			if(x >= 0 && y >= 0 && x < (LONG)data->Width && y < (LONG)data->Height)
			{
				struct HitTestMessage hmsg(x + data->Left, y + data->Top, this);
				if(HitTest(hmsg))
				{
					link = hmsg.URL;
					target = FindTarget(obj, hmsg.Target, data);

					linkobj = hmsg.Obj;
					url = (STRPTR)DoMethod(obj, MUIM_HTMLview_AddPart, link);
					if(hmsg.ServerMap)
					{
						STRPTR t_url = new char[strlen(url) + 11];
						sprintf(t_url, "%s?%ld,%ld", url, hmsg.OffsetX, hmsg.OffsetY);
						delete url;
						url = t_url;
						map = TRUE;
					}
				}
			}

			Object *dst = data->Share->Obj;
			if(imsg->Class == IDCMP_MOUSEMOVE || imsg->Class == IDCMP_INTUITICKS)
			{
				if(imsg->Qualifier & IEQUALIFIER_LEFTBUTTON && (data->Flags & FLG_MarkingEnabled) && !data->ActiveURL)
					Mark(x + data->Left, y + data->Top);

				/* Check which URL the mouse is ontop */
				if(link != data->LastURL || map)
				{
					SetAttrs(dst,
						MUIA_HTMLview_CurrentURL, url,
						MUIA_HTMLview_Target, target,
						TAG_DONE);

					if(data->ActiveURLObj)
					{
						if(data->LastURL == data->ActiveURL)
						{
							RedrawLink(obj, data->ActiveURLObj, data);
						}
						else
						{
							if(link == data->ActiveURL)
							{
								data->Flags |= FLG_ActiveLink;
								RedrawLink(obj, data->ActiveURLObj, data);
							}
						}
					}
				}
			}
			else
			{
				if((imsg->Code & ~IECODE_UP_PREFIX) == IECODE_LBUTTON)
				{
					if(imsg->Code & IECODE_UP_PREFIX)
					{
						if(url && data->ActiveURL == link)
						{
							if(data->ActiveURLObj)
								RedrawLink(obj, data->ActiveURLObj, data);

							SetAttrs(dst,
								MUIA_HTMLview_ClickedURL, url,
	 							MUIA_HTMLview_Target, target,
								MUIA_HTMLview_Qualifier, imsg->Qualifier,
	 							TAG_DONE);
	 					}
	 					data->ActiveURLObj = NULL;

						if(data->Flags & FLG_MarkingEnabled)
						{
							Copy();
							UnMark();
							data->Flags &= ~FLG_MarkingEnabled;
							SetAttrs(data->Share->Obj, MUIA_HTMLview_IntuiTicks, FALSE, TAG_DONE);
						}

/*						struct ChildsDList *first = MarkMsg->FirstMarkedChild;
						while(first)
						{
							class TextClass *text = first->Obj;
							STRPTR contents = text->Contents;
							contents += text->MarkBegin;
							LONG len = text->MarkEnd - text->MarkBegin;
							while(len--)
								printf("%c", *contents++);
							first = first->Next;
						}
						printf("\n");
*/	 				}
	 				else
	 				{
	 					if((data->ActiveURL = link)) // && linkobj->ID == tag_Text)
	 					{
							data->Flags |= FLG_ActiveLink;
							data->ActiveURLObj = linkobj;
							RedrawLink(obj, linkobj, data);
						}
						else
						{
							if(x >= 0 && y >= 0 && x < (LONG)data->Width && y < (LONG)data->Height)
							{
								class TextClass *last;
								if((last = FindMsg.LastMatch))
								{
									MarkMsg.AddBox(last, last->MarkBegin, last->MarkEnd);
									last->MarkBegin = last->MarkEnd = 0;
									data->Flags |= FLG_RedrawMarked;
									MUI_Redraw(obj, MADF_DRAWUPDATE);
									data->Flags &= ~FLG_RedrawMarked;
								}

								MarkMsg.Reset(x + data->Left, y + data->Top);
								data->Flags |= FLG_MarkingEnabled;
								SetAttrs(data->Share->Obj, MUIA_HTMLview_IntuiTicks, TRUE, TAG_DONE);
							}
						}
	 				}
				}
			}
			delete url;
			data->LastURL = link;
		}
	}
	return(result);
}

BOOL HostClass::HitTest (struct HitTestMessage &hmsg)
{
	BOOL result = FALSE;
	if(Body)
	{
		result = Body->HitTest(hmsg);
		if(!hmsg.Target)
			hmsg.Target = (Base && Base->Target) ? Base->Target : Data->FrameName;
	}
	return(result);
}

BOOL HostClass::Layout (struct LayoutMessage &lmsg)
{
	if(Body)
	{
		Body->Layout(lmsg);
	}
	else
	{
		if((Body = FindBody(FirstChild)))
			Layout(lmsg);
	}

  return TRUE;
}

VOID HostClass::Mark (LONG x, LONG y)
{
	MarkMsg.LastX = MarkMsg.X2;
	MarkMsg.LastY = MarkMsg.Y2;
	MarkMsg.X2 = x;
	MarkMsg.Y2 = y;
	MarkMsg.Flags = 0;
	ULONG x1 = MarkMsg.X1, y1 = MarkMsg.Y1;

	Body->Mark(MarkMsg);
	if(MarkMsg.FirstBox)
	{
		Data->Flags |= FLG_RedrawMarked;
		MUI_Redraw(Obj, MADF_DRAWUPDATE);
		Data->Flags &= ~FLG_RedrawMarked;
	}

	/* Restore coords, incase of tables... */
	MarkMsg.X1 = x1;
	MarkMsg.Y1 = y1;
	MarkMsg.X2 = x;
	MarkMsg.Y2 = y;
}

VOID HostClass::Copy ()
{
	MarkMsg.LastX = MarkMsg.X2;
	MarkMsg.LastY = MarkMsg.Y2;
	MarkMsg.Flags = 0;
	MarkMsg.Copy = TRUE;

	if(MarkMsg.X1 != MarkMsg.X2 || MarkMsg.Y1 != MarkMsg.Y2)
	{
		if(MarkMsg.OpenTheClipboard())
		{
			const ULONG len = 6*sizeof(ULONG);
			UBYTE backup[len];
			memcpy(backup, &MarkMsg.X1, len);
			Body->Mark(MarkMsg);
			memcpy(&MarkMsg.X1, backup, len);
			MarkMsg.CloseTheClipboard();
		}
	}
	MarkMsg.Copy = FALSE;
}

VOID HostClass::UnMark ()
{
	Mark(MarkMsg.X1, MarkMsg.Y1);
}

ULONG HostClass::NumPadMovement(struct IntuiMessage *imsg)
{
	struct InputEvent event;
	unsigned char code = 0;

	event.ie_NextEvent      = NULL;
	event.ie_Class          = IECLASS_RAWKEY;
	event.ie_SubClass       = 0;
	event.ie_Code           = imsg->Code;
	event.ie_Qualifier      = imsg->Qualifier;
	event.ie_EventAddress   = (APTR *) *((ULONG *)imsg->IAddress);

	MapRawKey(&event, (STRPTR)&code, 1, NULL);

	ULONG result;
	switch(code)
	{
		case '8':
			result = MUIKEY_UP;
		break;

		case '2':
			result = MUIKEY_DOWN;
		break;

		case '4':
			result = MUIKEY_LEFT;
		break;

		case '6':
			result = MUIKEY_RIGHT;
		break;

		case '9':
			result = MUIKEY_PAGEUP;
		break;

		case '3':
			result = MUIKEY_PAGEDOWN;
		break;

		case '7':
			result = MUIKEY_TOP;
		break;

		case '1':
			result = MUIKEY_BOTTOM;
		break;

		default:
			result = (ULONG)MUIKEY_NONE;
		break;
	}
	return(result);
}

BOOL HostClass::MatchKey(struct IntuiMessage *imsg, struct InputEvent &event, struct InputXpression &key)
{
	event.ie_Code           = imsg->Code;
	event.ie_Qualifier      = imsg->Qualifier;
	event.ie_EventAddress   = (APTR *) *((ULONG *)imsg->IAddress);
	return(MatchIX(&event, &key));
}

VOID HostClass::RedrawLink(Object *obj, class SuperClass *redrawobj, struct HTMLviewData *data)
{
	data->RenderMsg.RedrawLink = TRUE;
	data->RedrawObj = redrawobj;
	MUI_Redraw(obj, MADF_DRAWUPDATE);
	data->RenderMsg.RedrawLink = FALSE;
}

class TreeClass *HostClass::FindBody(struct ChildsList *first)
{
	while(first)
	{
		if(first->Obj->id() == tag_BODY || first->Obj->id() == tag_FRAMESET)
		{
			return((class TreeClass *)first->Obj);
		}
		else
		{
			if(first->Obj->flags() & FLG_Tree)
			{
				class TreeClass *body;
				if((body = FindBody(((class TreeClass *)first->Obj)->FirstChild)))
					return(body);
			}
		}
		first = first->Next;
	}

	return(NULL);
}

