/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005 by HTMLview.mcc Open Source Team

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
#include <stdio.h>

#include <clib/alib_protos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include "muiextra.h"
#include "private.h"

struct PrefsExchangeData { ULONG ObjIndex, Tag, CfgItem; LONG Length; const char *DefValue; };

static struct PrefsExchangeData PrefsInfo[] =
{
	{ SmallFont,			  MUIA_String_Contents,	MUICFG_HTMLview_SmallFont,				 0, "XHelvetica/9"						     },
	{ NormalFont,			  MUIA_String_Contents,	MUICFG_HTMLview_NormalFont,			   0, "XHelvetica/11"						     },
	{ FixedFont,			  MUIA_String_Contents,	MUICFG_HTMLview_FixedFont,				 0, "XEN/11"								       },
	{ LargeFont,			  MUIA_String_Contents,	MUICFG_HTMLview_LargeFont,				 0, "XHelvetica/13"						     },
	{ H1,						    MUIA_String_Contents,	MUICFG_HTMLview_H1,						     0, "Helvetica/24"						     },
	{ H2,						    MUIA_String_Contents,	MUICFG_HTMLview_H2,						     0, "Helvetica/18"						     },
	{ H3,						    MUIA_String_Contents,	MUICFG_HTMLview_H3,						     0, "Helvetica/15"						     },
	{ H4,						    MUIA_String_Contents,	MUICFG_HTMLview_H4,						     0, "XHelvetica/13"						     },
	{ H5,						    MUIA_String_Contents,	MUICFG_HTMLview_H5,						     0, "XHelvetica/11"						     },
	{ H6,						    MUIA_String_Contents,	MUICFG_HTMLview_H6,						     0, "XHelvetica/9"						     },
	{ IgnoreDocCols,	  MUIA_Selected,				MUICFG_HTMLview_IgnoreDocCols,		-4, (STRPTR)FALSE							    },
	{ Col_Background,	  MUIA_Pendisplay_Spec,	MUICFG_HTMLview_Col_Background,		32, "m1"										      },
	{ Col_Text,				  MUIA_Pendisplay_Spec,	MUICFG_HTMLview_Col_Text,				  32, "m5"										      },
	{ Col_Link,				  MUIA_Pendisplay_Spec,	MUICFG_HTMLview_Col_Link,				  32, "r00000000,00000000,99999999"	},
	{ Col_VLink,			  MUIA_Pendisplay_Spec,	MUICFG_HTMLview_Col_VLink,				32, "rAAAAAAAA,00000000,00000000"	},
	{ Col_ALink,			  MUIA_Pendisplay_Spec,	MUICFG_HTMLview_Col_ALink,				32, "rDCDCDCDC,DCDCDCDC,00000000"	},
	{ ListItemFile,		  MUIA_String_Contents,	MUICFG_HTMLview_ListItemFile,			 0, "PROGDIR:Images/ListItems"		},
	{ DitherType,			  MUIA_Cycle_Active,		MUICFG_HTMLview_DitherType,			  -4, (STRPTR)0								      },
	{ ImageCacheSize,		MUIA_Numeric_Value,		MUICFG_HTMLview_ImageCacheSize,		-4, (STRPTR)512							      },
	{ GammaCorrection,	MUIA_Numeric_Value,		MUICFG_HTMLview_GammaCorrection,	-4, (STRPTR)2200							    },
	{ PageScrollSmooth,	MUIA_Selected,				MUICFG_HTMLview_PageScrollSmooth,	-4, (STRPTR)TRUE							    },
	{ PageScrollKey,		MUIA_String_Contents,	MUICFG_HTMLview_PageScrollKey,		 0, "-repeat space"						    },
	{ PageScrollMove,		MUIA_Numeric_Value,		MUICFG_HTMLview_PageScrollMove,		-4, (STRPTR)70								    },
	{ -1, 0, 0, 0, NULL }
};

DISPATCHER(_DispatcherP)
{
	struct InstData_MCP *data = (struct InstData_MCP *)INST_DATA(cl, obj);

	ULONG result = 0;
	switch(msg->MethodID)
	{
		case OM_NEW:
		{
			if((obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg)))
			{
				struct InstData_MCP *data = (struct InstData_MCP *)INST_DATA(cl, obj);

				Object *group;
				if((group = CreatePrefsGroup(data)))
				{
					ULONG i;

          DoMethod(obj, OM_ADDMEMBER, group);

					for(i = 0; i < NumberOfObjects; i++)
						DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->Objects[PrefsInfo[i].ObjIndex], PrefsInfo[i].CfgItem, 0L, NULL);

					result = (ULONG)obj;
				}
				else
				{
					CoerceMethod(cl, obj, OM_DISPOSE);
				}
			}
		}
		break;

		case MUIM_Settingsgroup_ConfigToGadgets:
		{
			Object *configdata = ((struct MUIP_Settingsgroup_ConfigToGadgets *)msg)->configdata;

			struct PrefsExchangeData *item = PrefsInfo;
			while((LONG)item->ObjIndex != -1)
			{
				STRPTR cfg_val;
				if((cfg_val = (STRPTR)DoMethod(configdata, MUIM_Dataspace_Find, item->CfgItem)))
				{
					if(item->Length < 0)
						cfg_val = *(STRPTR *)cfg_val;
				}
				else
          cfg_val = (STRPTR)item->DefValue;

				set(data->Objects[item->ObjIndex], item->Tag, cfg_val);
				item++;
			}
		}
		break;

		case MUIM_Settingsgroup_GadgetsToConfig:
		{
			Object *configdata = ((struct MUIP_Settingsgroup_ConfigToGadgets *)msg)->configdata;

			struct PrefsExchangeData *item = PrefsInfo;
			while((LONG)item->ObjIndex != -1)
			{
				STRPTR cfg_val;
				LONG len;

				cfg_val = (STRPTR)xget(data->Objects[item->ObjIndex], item->Tag);

				if((len = item->Length) < 0)
				  DoMethod(configdata, MUIM_Dataspace_Add, &cfg_val, 4, item->CfgItem);
				else
          DoMethod(configdata, MUIM_Dataspace_Add, cfg_val, len ? len : (LONG)strlen(cfg_val)+1, item->CfgItem);

				item++;
			}
		}
		break;

		default:
			result = DoSuperMethodA(cl, obj, msg);
		break;
	}
	return result;
}

struct CacheSliderData
{
	char Buffer[16];
};

DISPATCHER(CacheSliderDispatcher)
{
	ULONG result;
	if(msg->MethodID == MUIM_Numeric_Stringify)
	{
		struct CacheSliderData *data = (struct CacheSliderData *)INST_DATA(cl, obj);
		struct MUIP_Numeric_Stringify *smsg = (struct MUIP_Numeric_Stringify *)msg;
		char *buf = data->Buffer;

		ULONG val = smsg->value;
		if(val < 103)
		  snprintf(buf, sizeof(data->Buffer), "%ld KB", val);
		else
      snprintf(buf, sizeof(data->Buffer), "%ld.%ld MB", val/1024, (10*(val%1024))/1024);

		result = (ULONG)buf;
	}
	else
		result = DoSuperMethodA(cl, obj, msg);

	return result;
}

struct GammaSliderData
{
	char Buffer[16];
};

DISPATCHER(GammaSliderDispatcher)
{
	ULONG result;
	if(msg->MethodID == MUIM_Numeric_Stringify)
	{
		struct GammaSliderData *data = (struct GammaSliderData *)INST_DATA(cl, obj);
		struct MUIP_Numeric_Stringify *smsg = (struct MUIP_Numeric_Stringify *)msg;
		char *buf = data->Buffer;

		ULONG val = smsg->value;
		snprintf(buf, sizeof(data->Buffer), "%ld.%03ld", val/1000, val%1000);

		result = (ULONG)buf;
	}
	else
	{
		result = DoSuperMethodA(cl, obj, msg);
	}
	return result;
}

struct MUI_CustomClass *CacheSliderClass = NULL;
struct MUI_CustomClass *GammaSliderClass = NULL;

BOOL CreateSubClasses(void)
{
  if((CacheSliderClass = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL, sizeof(struct CacheSliderData), ENTRY(CacheSliderDispatcher))))
  {
    if((GammaSliderClass = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL, sizeof(struct GammaSliderData), ENTRY(GammaSliderDispatcher))))
    {
      return TRUE;
    }
  }

  return FALSE;
}

void DeleteSubClasses(void)
{
  if(GammaSliderClass)
  {
    MUI_DeleteCustomClass(GammaSliderClass);
    GammaSliderClass = NULL;
  }

  if(CacheSliderClass)
  {
    MUI_DeleteCustomClass(CacheSliderClass);
    CacheSliderClass = NULL;
  }
}
