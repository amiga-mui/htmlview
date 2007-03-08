
#include "TreeClass.h"
#include "TextClass.h"

#include "General.h"
#include "Layout.h"
#include "Map.h"
#include "MinMax.h"
#include "ParseMessage.h"

TreeClass::~TreeClass ()
{
	struct ChildsList *prev, *first = FirstChild;
	while(first)
	{
		prev = first;
		first = first->Next;
		delete prev->Obj;
		delete prev;
	}
}

class SuperClass *TreeClass::FindElement (ULONG tagID)
{
	class SuperClass *result = SuperClass::FindElement(tagID);
	struct ChildsList *first = FirstChild;
	while(first && !result)
	{
		result = first->Obj->FindElement(tagID);
		first = first->Next;
	}
	return(result);
}

VOID TreeClass::AddChild (class SuperClass *obj)
{
	struct ChildsList *newchild;
	newchild = new struct ChildsList(obj);
	Last->Next = newchild;
	Last = newchild;
}

VOID TreeClass::AllocateColours (struct ColorMap *cmap)
{
	struct ChildsList *first = FirstChild;
	while(first)
	{
		/* Why comment the next two lines? */
		if((first->Obj->flags() & (FLG_Tree | FLG_ArgumentsRead)) == FLG_Tree)
			break;

		first->Obj->AllocateColours(cmap);
		first = first->Next;
	}
}

VOID TreeClass::FreeColours (struct ColorMap *cmap)
{
	struct ChildsList *first = FirstChild;
	while(first)
	{
		first->Obj->FreeColours(cmap);
		first = first->Next;
	}
}

VOID TreeClass::AppendGadget (struct AppendGadgetMessage &amsg)
{
	struct ChildsList *first = FirstChild;
	while(first)
	{
		first->Obj->AppendGadget(amsg);
		first = first->Next;
	}
}

class TextClass *TreeClass::Find (struct FindMessage &fmsg)
{
	class TextClass *result = NULL;

	struct ChildsList *first = FirstChild;
	while(first && !result)
	{
		result = first->Obj->Find(fmsg);
		first = first->Next;
	}
	return(result);
}

VOID TreeClass::ExportForm (struct ExportFormMessage &emsg)
{
	struct ChildsList *first = FirstChild;
	while(first)
	{
		first->Obj->ExportForm(emsg);
		first = first->Next;
	}
}

VOID TreeClass::ResetForm ()
{
	struct ChildsList *first = FirstChild;
	while(first)
	{
		first->Obj->ResetForm();
		first = first->Next;
	}
}

VOID TreeClass::GetImages (struct GetImagesMessage &gmsg)
{
	struct ChildsList *first = FirstChild;
	while(first)
	{
		first->Obj->GetImages(gmsg);
		first = first->Next;
	}
}

BOOL TreeClass::HitTest (struct HitTestMessage &hmsg)
{
	LONG x = hmsg.X, y = hmsg.Y;
	BOOL result = FALSE;
	struct ChildsList *first = FirstChild;
	while(first && !result)
	{
		if(first->Obj->flags() & FLG_Tree || (y >= first->Obj->top() && y <= first->Obj->bottom()))
		{
			result = first->Obj->HitTest(hmsg);
		}
		else
		{
			if(y < first->Obj->top() && (first->Obj->flags() & FLG_Newline))
				break;
		}
		first = first->Next;
	}
	return(result);
}

class AClass *TreeClass::FindAnchor (STRPTR name)
{
	class AClass *result = NULL;
	struct ChildsList *first = FirstChild;
	while(first && !result)
	{
		if(first->Obj->flags() & FLG_Layouted)
				result = first->Obj->FindAnchor(name);
		else	break;
		first = first->Next;
	}
	return(result);
}

VOID TreeClass::Relayout (BOOL all)
{
	struct ChildsList *first = FirstChild;
	while(first)
	{
		first->Obj->Relayout(all);
		first = first->Next;
	}
	SuperClass::Relayout(all);
	Flags |= FLG_Virgin;
}

BOOL TreeClass::Layout (struct LayoutMessage &lmsg)
{
//	StackCheck();

	if(Flags & FLG_Virgin)
	{
		Top = lmsg.Y;
		Bottom = 0;
		Flags &= ~FLG_Virgin;
	}

	BOOL complete = Flags & FLG_AllElementsPresent, waiting = FALSE;
	struct ChildsList *first = FirstChild;
	while(first)
	{
		if((first->Obj->flags() & (FLG_Tree | FLG_ArgumentsRead)) == FLG_Tree)
			break;

		if(!(first->Obj->flags() & (FLG_Layouted | FLG_WaitingForSize)))
		{
			lmsg.Parent = this;
			first->Obj->Layout(lmsg);
			if(!(first->Obj->flags() & (FLG_Layouted | FLG_WaitingForSize)))
			{
				complete = FALSE;
				break;
			}
			Bottom = max(Bottom, first->Obj->bottom());
		}
		first = first->Next;
	}

	if(complete)
	{
		Bottom = max(Bottom, lmsg.Y + lmsg.Baseline + lmsg.Bottom - 1);

		struct FloadingImage *img = lmsg.FLeft;
		while(img)
		{
			if(img->Container == (class SuperClass *)this)
				Bottom = max(Bottom, img->Top + img->Height);
			img = img->Next;
		}

		img = lmsg.FRight;
		while(img)
		{
			if(img->Container == (class SuperClass *)this)
				Bottom = max(Bottom, img->Top + img->Height);
			img = img->Next;
		}

		Flags |= FLG_Layouted;
	}
	return(complete);
}

VOID TreeClass::AdjustPosition (LONG x, LONG y)
{
	struct ChildsList *first = FirstChild;
	while(first)
	{
		first->Obj->AdjustPosition(x, y);
		first = first->Next;
	}
	SuperClass::AdjustPosition(x, y);
}

class MapClass *TreeClass::FindMap (STRPTR name)
{
	class MapClass *result = NULL;
	struct ChildsList *first = FirstChild;
	while(first && !result)
	{
		result = first->Obj->FindMap(name);
		first = first->Next;
	}
	return(result);
}

BOOL TreeClass::UseMap (struct UseMapMessage &umsg)
{
	BOOL result = RC_NoMatch, def_match = FALSE;;
	struct ChildsList *first = FirstChild;
	while(first && result != RC_ExactMatch)
	{
		if((result = first->Obj->UseMap(umsg)) == RC_Default)
			def_match = TRUE;
		first = first->Next;
	}
	return(def_match ? RC_ExactMatch : result);
}

BOOL TreeClass::Mark (struct MarkMessage &mmsg)
{
	if(mmsg.Y1 > Bottom && mmsg.Y2 > Bottom && mmsg.LastY > Bottom)
		return(FALSE);

	BOOL result = SuperClass::Mark(mmsg);
	if(Flags & FLG_Newline)
	{
		if(!mmsg.Newline && mmsg.Enabled())
			mmsg.WriteLF();
		mmsg.Newline = TRUE;
	}

	struct ChildsList *first = FirstChild;
	while(first && !result)
	{
		result = first->Obj->Mark(mmsg);
		first = first->Next;
	}

	if(Flags & FLG_Newline)
	{
		if(!mmsg.Newline && mmsg.Enabled())
			mmsg.WriteLF();
		mmsg.Newline = TRUE;
	}
	return(result);
}

VOID TreeClass::MinMax (struct MinMaxMessage &mmsg)
{
	if(Flags & FLG_Newline)
		mmsg.Newline();

	struct ChildsList *first = FirstChild;
	while(first)
	{
		first->Obj->MinMax(mmsg);
		first = first->Next;
	}

	if(Flags & FLG_Newline)
		mmsg.Newline();

	Flags |= FLG_KnowsMinMax;
}

VOID TreeClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
//	StackCheck();

	Flags |= FLG_ArgumentsRead;

	class SuperClass *newobj;
	struct TagInfo *tinfo;
	BOOL repeat;

/*	APTR handle = NULL;
	if(Flags & FLG_Newline)
		handle = Backup(pmsg, 1, tag_P);
*/

	if(Flags & FLG_Newline)
		pmsg.PendingSpace = FALSE;

	pmsg.OpenCounts[ID]++;
	pmsg.OpenGroups[Group]++;
	if(!pmsg.OpenCounts[tag_PRE])
	{
		pmsg.SkipSpaces();
		pmsg.PendingSpace = FALSE;
	}
	else
	{
		if(ID == tag_PRE)
		{
			pmsg.Fetch();
			if(*pmsg.Current == '\n' || *pmsg.Current == '\r')
				pmsg.Current++;
			pmsg.PendingSpace = FALSE;
			pmsg.PendingNewline = FALSE;
		}
	}

	BOOL weak_nl = FALSE, space = FALSE;
	do {

		repeat = FALSE;
		while(pmsg.Fetch(12) && (pmsg.Current[1] != '/' || *pmsg.Current != '<'))
		{
			if(*pmsg.Current == '<')
			{
				tinfo = GetTagInfo(pmsg.Current+1);
				if(tinfo->ID == tag_Unknown)
				{
					pmsg.NextEndBracket();
				}
				else
				{
					if((tinfo->Flags & FLG_NoNesting && pmsg.OpenCounts[tinfo->ID]) || (tinfo->Flags & FLG_NoGroupNesting && pmsg.OpenGroups[tinfo->Group]) || (pmsg.OpenCounts[tag_P] && tinfo->Flags & FLG_Blocklevel))
					{
#ifdef OUTPUT
						printf("</%s> (implicit) %d (new: %s)\n", TagTable[ID].Name, pmsg.OpenCounts[ID], TagTable[tinfo->ID].Name);
#endif
						if(pmsg.OpenCounts[ID])
						{
							pmsg.OpenCounts[ID]--;
							pmsg.OpenGroups[Group]--;
						}
						return;
					}

					newobj = CreateObject(tinfo);
					if(newobj->flags() & FLG_Attribute)
					{
						newobj->Parse(pmsg);
						AddChild(newobj);
						weak_nl = TRUE;
					}
					else
					{
						AddChild(newobj);
						newobj->Parse(pmsg);
						newobj->setFlags(newobj->flags() | FLG_AllElementsPresent);
					}

					if(newobj->flags() & FLG_Newline)
					{
						pmsg.PendingNewline = TRUE;
						weak_nl = FALSE;
					}
				}
			}

			if(pmsg.PendingNewline)
			{
				pmsg.PendingSpace = FALSE;
				STRPTR a = pmsg.Current;
				pmsg.SkipSpaces();
				if(a < pmsg.Current)
					space = TRUE;
			}

			if(*pmsg.Current != '<')
			{
				pmsg.SetLock();
				pmsg.NextStartBracket();

				ULONG length = pmsg.Current-pmsg.Locked;

				pmsg.Fetch();
				if(pmsg.Current[1] == '/')
				{
					if(pmsg.OpenCounts[tag_PRE])
					{
						UBYTE c = pmsg.Locked[length-1];
						if(length && (c == '\n' || c == '\r'))
							length--;
					}
					else
					{
						while(length && IsWhitespace(pmsg.Locked[length-1]))
							length--;
					}
				}

				if(length)
				{
					if(weak_nl && space)
						pmsg.PendingSpace = TRUE;

					newobj = new class TextClass();
					STRPTR current = pmsg.Current;
					pmsg.Current = pmsg.Locked + length;
					newobj->Parse(pmsg);
					AddChild(newobj);
					pmsg.Current = current;
					pmsg.PendingNewline = FALSE;
				}
			}
		}

		pmsg.Fetch(12);
		tinfo = GetTagInfo(pmsg.Current+2);

#ifdef OUTPUT
		if(ID == tinfo->ID)
		{
			printf("</%s> (%d)\n", tinfo->Name, pmsg.OpenCounts[ID]);
		}
		else
		{
			if(pmsg.OpenCounts[tinfo->ID])
					printf("</%s> (implicit) %d (new: <%s>)\n", TagTable[ID].Name, pmsg.OpenCounts[tinfo->ID], TagTable[tinfo->ID].Name);
			else	printf("</%s> (skipping)\n", tinfo->Name);
			PrintTag(pmsg.Current);
		}
#endif

		if(ID == tinfo->ID)
		{
			pmsg.NextEndBracket();
			if(!*pmsg.Current)
				*pmsg.Current = '<';
			pmsg.OpenCounts[ID]--;
			pmsg.OpenGroups[Group]--;
		}
		else
		{
			if(pmsg.OpenCounts[tinfo->ID])
			{
				pmsg.OpenCounts[ID]--;
				pmsg.OpenGroups[Group]--;
			}
			else
			{
				pmsg.NextEndBracket();
				if(!*pmsg.Current)
					*pmsg.Current = '<';
				repeat = TRUE;
			}
		}

	}	while(repeat && pmsg.Fetch());

	if(Flags & FLG_Newline && !pmsg.OpenCounts[tag_PRE])
	{
		pmsg.SkipSpaces();
		pmsg.PendingSpace = FALSE;
		pmsg.PendingNewline = TRUE;
	}

/*	if(handle)
		Restore(pmsg.OpenCounts, 1, handle);
*/
}

VOID TreeClass::Render (struct RenderMessage &rmsg)
{
	struct ChildsList *first = FirstChild;
	while(first)
	{
		if(first->Obj->flags() & (FLG_Layouted | FLG_Tree))
		{
			WORD place = first->Obj->InDomain(rmsg);
			if((place == InsideLayer) || (!(first->Obj->flags() & FLG_Layouted) && (first->Obj->flags() & FLG_ArgumentsRead)))
			{
				if(!rmsg.TargetObj || (first->Obj->flags() & FLG_Tree) || rmsg.TargetObj == first->Obj)
					first->Obj->Render(rmsg);
			}
			else
			{
				if(place == BelowLayer && (first->Obj->flags() & FLG_Newline))
					return;
			}
			first = first->Next;
		}
		else
		{
			return;
		}
	}
}

VOID TreeClass::SetCol(LONG &storage, ULONG pen)
{
	if(pen != -1)
		storage = pen;
}

APTR TreeClass::Backup(struct ParseMessage &pmsg, ULONG len, ULONG tags, ...)
{
	ULONG *actual = &tags;
	UWORD *result;
	result = new UWORD [2*len];
	UWORD *res = result;
	while(len--)
	{
		*res++ = pmsg.OpenCounts[*actual];
		*res++ = *actual;
		pmsg.OpenCounts[*actual++] = 0;
	}
	return(result);
}

VOID TreeClass::Restore(UBYTE *opencounts, ULONG len, APTR handle)
{
	UWORD *counts = (UWORD *)handle;
	while(len--)
		opencounts[*counts++] = *counts++;
	delete handle;
}

LONG TreeClass::AllocPen(struct ColorMap *cmap, LONG t_rgb)
{
	if(t_rgb == -1)
		return(-1);

	UBYTE *rgb = (UBYTE *)&t_rgb;
	ULONG r, g, b;
	r = Precision(rgb[1]);
	g = Precision(rgb[2]);
	b = Precision(rgb[3]);

	return(ObtainBestPenA(cmap, r, g, b, NULL));
}

