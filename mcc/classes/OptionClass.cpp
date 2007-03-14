
#include "OptionClass.h"
#include "TextClass.h"

#include "ParseMessage.h"
#include "ScanArgs.h"

VOID OptionClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	BOOL disabled = FALSE, selected = FALSE;
	struct ArgList args[] =
	{
		{ "VALUE",		&Value,		ARG_STRING, NULL	},
		{ "DISABLED",	&disabled,	ARG_SWITCH, NULL	},
		{ "SELECTED",	&selected,	ARG_SWITCH, NULL	},
		{ NULL,        NULL,       0,          NULL  }
	};

	ScanArgs(pmsg.Locked, args);

	if(disabled)
		Flags |= FLG_Select_Disabled;
	if(selected)
		Flags |= FLG_Select_Multiple;

	TreeClass::Parse(pmsg);
	if(FirstChild && FirstChild->Obj->id() == tag_Text)
	{
		if((Contents = ((class TextClass *)FirstChild->Obj)->Contents))
		{
			STRPTR contents = Contents;
			while(*contents)
			{
				if(*contents == '\n')
						*contents = '\0';
				else	contents++;
			}

			while(contents > Contents && *--contents == ' ')
				*contents = '\0';
		}
	}

/*	pmsg.SetLock();
	pmsg.NextStartBracket();

	if(Contents = new UBYTE [pmsg.Current-pmsg.Locked+1])
	{
		STRPTR src = pmsg.Locked, dst = Contents;
		while(*src != '<' && *src != '\n' && *src != '\r' && *src)
			*dst++ = *src++;
		*dst = '\0';
	}
*/
}
