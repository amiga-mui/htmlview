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

#include "InputClass.h"
#include "FormClass.h"

#include "Forms.h"
#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "private.h"
#include "ScanArgs.h"

#include "SDI_hook.h"

#include <proto/muimaster.h>
#include <mui/BetterString_mcc.h>

HOOKPROTONHNO(ExportFormCode, VOID, class FormClass **form)
{
	(*form)->ExportForm(*((struct ExportFormMessage *)NULL));
}
MakeStaticHook(ExportFormHook, ExportFormCode);

VOID InputClass::AppendGadget (struct AppendGadgetMessage &amsg)
{
	if(!MUIGadget)
	{
		Object *obj = NULL;
		switch(Type)
		{
			case Input_Text:
				obj = BetterStringObject,
					StringFrame,
					MUIA_BetterString_Columns, Size ? Size : 40,
					MUIA_String_MaxLen, MaxLength,
					MUIA_String_AdvanceOnCR, TRUE,
					MUIA_String_Contents, Value,
					End;
			break;

			case Input_Password:
				obj = BetterStringObject,
					StringFrame,
					MUIA_BetterString_Columns, Size ? Size : 40,
					MUIA_String_MaxLen, MaxLength,
					MUIA_String_AdvanceOnCR, TRUE,
					MUIA_String_Contents, Value,
					MUIA_String_Secret, TRUE,
					End;
			break;

			case Input_Checkbox:
				obj = ImageObject,
					ImageButtonFrame,
					MUIA_InputMode,		MUIV_InputMode_Toggle,
					MUIA_Image_Spec,		MUII_CheckMark,
					MUIA_Image_FreeVert,	TRUE,
					MUIA_Background,		MUII_ButtonBack,
					MUIA_ShowSelState,	FALSE,
					End;

				if(Flags & FLG_Input_Checked)
				{
					SetAttrs(obj,
						MUIA_Selected,		TRUE,
						MUIA_Image_State,	TRUE,
						TAG_DONE);
				}
			break;

			case Input_Radio:
			break;

			case Input_Reset:
			break;

			case Input_File:
			break;

			case Input_Image:
			break;

/*			case Input_Hidden:
			break;
*/
			case Input_Button:
				obj = SimpleButton(Value);
			break;

			case Input_Submit:
			{
				obj = SimpleButton(Value ? Value : "Submit");
				if(amsg.Form)
					DoMethod(obj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Application, 6, MUIM_Application_PushMethod, _app(amsg.Parent), 3, MUIM_CallHook, &ExportFormHook, amsg.Form);
			}
			break;
		}

		if((MUIGadget = obj))
		{
			SetAttrs(obj, MUIA_CycleChain, TRUE, TAG_DONE);
			DoMethod(amsg.Parent, OM_ADDMEMBER, MUIGadget);
		}
		else
			Flags |= FLG_Layouted;
	}
}

VOID InputClass::ExportForm (struct ExportFormMessage &emsg)
{
	STRPTR value = NULL;
	switch(Type)
	{
		case Input_Text:
		case Input_Password:
			if(MUIGadget)
				value = (STRPTR)xget(MUIGadget, MUIA_String_Contents);
		break;

		case Input_Checkbox:
			if(MUIGadget)
			{
				value = (STRPTR)xget(MUIGadget, MUIA_Selected);
				if(value)
					value = (STRPTR)"on"; /* ?¿ */
			}
		break;

		case Input_Radio:
		break;

		case Input_Reset:
		break;

		case Input_File:
		break;

		case Input_Hidden:
			value = Value;
		break;

		case Input_Image:
		break;

		case Input_Button:
		break;

		case Input_Submit:
		break;
	}

	if(Name && value)
		emsg.AddElement(Name, value);
}

VOID InputClass::ResetForm ()
{

}

BOOL InputClass::Layout (struct LayoutMessage &lmsg)
{
	if(!(Flags & FLG_Input_ObjAdded))
	{
		lmsg.AddToGadgetList(this);
		Flags |= FLG_Input_ObjAdded;
	}

	if(MUIGadget)
	{
		Width = _minwidth(MUIGadget) + 4;
		Height = _minheight(MUIGadget) + 2;

		if((LONG)Width > lmsg.ScrWidth())
			lmsg.EnsureNewline();

		Left = lmsg.X+2;
		lmsg.X += Width;

		Top = lmsg.Y - (Height-7);
		Bottom = lmsg.Y;

		lmsg.UpdateBaseline(Height, Height-7);

		struct ObjectNotify *notify = new struct ObjectNotify(Left, Baseline, this);
		lmsg.AddNotify(notify);

		Flags |= FLG_WaitingForSize;
	}
	else
	{
//		if(Type == Input_Hidden)
			Flags |= FLG_Layouted;
	}

   return TRUE;
}

VOID InputClass::AdjustPosition (LONG x, LONG y)
{
	Left += x;
	SuperClass::AdjustPosition(x, y);
	if(MUIGadget && Width && Height)
		MUI_Layout(MUIGadget, Left, Top, Width-4, Height-2, 0L);
}

VOID InputClass::MinMax (struct MinMaxMessage &mmsg)
{
	if(Type != Input_Hidden && MUIGadget)
	{
		ULONG width = _minwidth(MUIGadget);
		mmsg.X += width;
		mmsg.Min = max((LONG)width, mmsg.Min);
	}
}

VOID InputClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	AttrClass::Parse(pmsg);

	BOOL checked = FALSE, disabled = FALSE, readonly = FALSE;
	const char *InputTypes[] = { "TEXT", "PASSWORD", "CHECKBOX", "RADIO", "SUBMIT", "RESET", "FILE", "HIDDEN", "IMAGE", "BUTTON", NULL };
	struct ArgList args[] =
	{
		{ "TYPE",		    &Type,		  ARG_KEYWORD, InputTypes	},
		{ "NAME",		    &Name,		  ARG_STRING,  NULL },
		{ "VALUE",		  &Value,		  ARG_STRING,  NULL },
		{ "CHECKED",	  &checked,	  ARG_SWITCH,  NULL },
		{ "DISABLED",	  &disabled,	ARG_SWITCH,  NULL },
		{ "READONLY",	  &readonly,	ARG_SWITCH,  NULL },
		{ "SIZE",		    &Size,		  ARG_NUMBER,  NULL },
		{ "MAXLENGTH",	&MaxLength,	ARG_NUMBER,  NULL },
		{ NULL,         NULL,       0,           NULL }
	};

	ScanArgs(pmsg.Locked, args);

	if(checked)
		Flags |= FLG_Input_Checked;
	if(disabled)
		Flags |= FLG_Input_Disabled;
	if(readonly)
		Flags |= FLG_Input_ReadOnly;

	if(MaxLength)
		MaxLength++;

	pmsg.Gadgets = TRUE;
}

VOID InputClass::Render (struct RenderMessage &rmsg)
{
	if(rmsg.RedrawGadgets && MUIGadget)
		MUI_Redraw(MUIGadget, MADF_DRAWOBJECT | MADF_DRAWOUTER);
}


