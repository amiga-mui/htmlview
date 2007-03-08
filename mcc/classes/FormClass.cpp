
#include "FormClass.h"

#include "Forms.h"
#include "ParseMessage.h"
#include "private.h"
#include "ScanArgs.h"
#include "SharedData.h"

VOID FormClass::AppendGadget (struct AppendGadgetMessage &amsg)
{
//	class FormClass *oldform = amsg.Form;
	amsg.Form = this;
	TreeClass::AppendGadget(amsg);
//	amsg.Form = oldform;
}

VOID FormClass::ExportForm (struct ExportFormMessage &emsg)
{
	if(&emsg == NULL)
	{
		struct HTMLviewData *data;

		data = (struct HTMLviewData *)xget(HTMLview, MUIA_HTMLview_InstanceData);

		if(Method == Form_Get && Action)
		{
			struct ExportFormMessage emsg2;
			TreeClass::ExportForm(emsg2);

			STRPTR base = (STRPTR)DoMethod(HTMLview, MUIM_HTMLview_AddPart, Action);
			ULONG len = strlen(base) + emsg2.StrLength + 1;
			STRPTR url = new char[len];
			ULONG index = sprintf(url, "%s?", base);
			delete base;

			emsg2.GetElements(url+index);

			SetAttrs(data->Share->Obj,
					MUIA_HTMLview_ClickedURL, url,
					MUIA_HTMLview_Target, FindTarget(HTMLview, Target, data),
					MUIA_HTMLview_Qualifier, 0L,
					TAG_DONE);

			delete url;
		}
		else if(Method == Form_Post && Action)
		{
			struct ExportFormMessage emsg2;
			TreeClass::ExportForm(emsg2);

			STRPTR base = (STRPTR)DoMethod(HTMLview, MUIM_HTMLview_AddPart, Action);
			STRPTR contents = new char[emsg2.StrLength + 1];
			emsg2.GetElements(contents);

			DoMethod(HTMLview, MUIM_HTMLview_Post, base, Target, contents, EncType ? EncType : "application/x-www-form-urlencoded");
			delete base;
//			This is deleted by the ParseThread
//			delete contents;
		}
	}
	else
	{
		TreeClass::ExportForm(emsg);
	}
}

VOID FormClass::ResetForm ()
{
	TreeClass::ResetForm();
}

VOID FormClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	pmsg.SetLock();
	pmsg.NextEndBracket();
#ifdef OUTPUT
	PrintTag(pmsg.Locked);
#endif

	STRPTR MethodKeywords[] = { "GET", "POST", NULL };
	struct ArgList args[] =
	{
		{ "METHOD",		&Method,		ARG_KEYWORD, MethodKeywords	},
		{ "ACTION",		&Action,		ARG_URL		},
		{ "TARGET",		&Target,		ARG_URL		},
		{ "ENCTYPE",	&EncType,	ARG_STRING	},
		{ NULL }
	};
	ScanArgs(pmsg.Locked, args);

	HTMLview = pmsg.HTMLview;

	UBYTE p = pmsg.OpenCounts[tag_P];
	pmsg.OpenCounts[tag_P] = 0;
	TreeClass::Parse(pmsg);
	pmsg.OpenCounts[tag_P] = p;
}

