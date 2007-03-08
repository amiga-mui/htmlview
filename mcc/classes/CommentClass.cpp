
#include "CommentClass.h"

#include "ParseMessage.h"

VOID CommentClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
	BOOL out_of_comment = FALSE;
	pmsg.SetLock(); /* we don't want to buffer the entire comment */
	pmsg.Current += 4; /* skip '<!--' */
	pmsg.Current[-1] = '_';

	do {

		while(pmsg.Fetch() && *pmsg.Current != '-' && *pmsg.Current != '>')
			pmsg.Current++;

		if(*pmsg.Current == '-' && pmsg.Current[-1] == '-')
		{
			*pmsg.Current = '_';
			out_of_comment = TRUE; /* Should be a toggle */
		}
		else if(*pmsg.Current == '>' && out_of_comment)
		{
			pmsg.Current++;
			break;
		}
		pmsg.Current++;

	}	while(pmsg.Fetch());

#ifdef OUTPUT
	printf("<!-- -->\n");
#endif
}
