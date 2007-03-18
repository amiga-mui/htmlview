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
#include <ctype.h>
#include <proto/intuition.h>

#include "URLHandling.h"

#include "classes/BaseClass.h"
#include "classes/HostClass.h"

#include "private.h"

BOOL LocalURL (STRPTR url)
{
	return(!strncmp("file://", url, 7));
}

STRPTR HTMLview_AddPart (UNUSED Object *obj, struct MUIP_HTMLview_AddPart *amsg, struct HTMLviewData *data)
{
	STRPTR url, urlbase, base, file = amsg->File;
	urlbase = base = (data->HostObject && data->HostObject->Base && data->HostObject->Base->URL) ? data->HostObject->Base->URL : data->URLBase;
//	urlbase = base = data->URLBase;

	if(base)
	{
		BOOL running = TRUE;
		while(*file && running)
		{
			switch(*file++)
			{
				case ':':
				{
					url = new char[strlen(amsg->File) + 2];
					strcpy(url, amsg->File);

					if(!*file)
						return(url);

					if(!strncmp(url, "http://", 7))
					{
						STRPTR addr = url;
						ULONG slashes = 0;
						while(*addr && slashes < 3)
						{
							*addr = tolower(*addr);
							if(*addr++ == '/')
								slashes++;
						}
						if(slashes == 2)
						{
							*addr++ = '/';
							*addr = '\0';
						}
					}
					running = FALSE;
				}
				break;

				case '?':
				case '/':
					file += strlen(file); /* This will cause the loop to break */
				break;
			}
		}

		if(running)
		{
			file = amsg->File;
			UWORD len = strlen(base) + strlen(file) + 1;
			if(*file == '#')
			{
				url = new char[len + strlen(data->Page)];
				strcpy(url, urlbase);
				strcat(url, data->Page);
				strcat(url, file);
			}
			else
			{
				url = new char[len];
				strcpy(url, urlbase);

				UBYTE sep = LocalURL(url) ? ':' : '/';
				STRPTR root = url+7;
				while(*root && *root++ != sep)
					;

				while(*file)
				{
					if(*file == '/')
					{
						file++;
						*root = '\0';
					}
					else if(!strncmp(file, "./", 2))
					{
						file += 2;
					}
					else if(!strncmp(file, "../", 3))
					{
						file += 3;

						WORD len = -1, prev = -1;
						STRPTR t_url = root;
						while(*t_url)
						{
							if(*t_url == '/')
							{
								prev = len;
								len = t_url-root;
							}
							t_url++;
						}
						root[prev+1] = '\0';
					}
					else
					{
						STRPTR t_file = file;
						while(*t_file && *t_file++ != '/')
						{
							if(t_file[-1] == '?')
								t_file += strlen(t_file);
						}
						UWORD len = t_file - file;
						strncat(url, file, len);
						file += len;
					}
				}
			}
		}
	}
	else
	{
		url = new char[strlen(file) + 1];
		strcpy(url, file);
	}
	return(url);
}

VOID HTMLview_SetPath (UNUSED Object *obj, STRPTR url, struct HTMLviewData *data)
{
	/* Domain name */
	delete data->URLBase;
	data->URLBase = new char[strlen(url)+2];
	STRPTR dst = data->URLBase;

	if(LocalURL(url))
	{
		strncpy(dst, url, 7);
		url += 7;
		dst += 7;
		while(*url)
		{
			*dst++ = *url;
			if(*url++ == ':')
				break;
		}
	}
	else
	{
		ULONG slashes = 3;
		while(*url)
		{
			*dst++ = tolower(*url);
			if(*url++ == '/')
			{
				if(!--slashes)
					break;
			}
		}
		if(slashes)
			*dst++ = '/';
	}

	/* Path */
	ULONG t_len = 0, len = 0;
	while(url[t_len])
	{
		if(url[t_len++] == '/')
			len = t_len;
	}
	strncpy(dst, url, len);
	url += len;

	/* Page name */
	delete data->Page;
	data->Page = new char[strlen(url)+1];

	len = 0;
	while(url[len] && url[len] != '#')
		len++;
	if(len)
			strncpy(data->Page, url, len);
	else	*data->Page = '\0';
	url += len;

	/* Local anchor */
	delete data->Local;
	if(*url++ == '#')
	{
		data->Local = new char[strlen(url)+1];
		strcpy(data->Local, url);
	}
	else
	{
		data->Local = NULL;
	}
}
