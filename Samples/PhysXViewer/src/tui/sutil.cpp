/*----------------------------------------------------------------------
		Copyright (c) 2004 Open Dynamics Framework Group
					www.physicstools.org
		All rights reserved.

		Redistribution and use in source and binary forms, with or without modification, are permitted provided
		that the following conditions are met:

		Redistributions of source code must retain the above copyright notice, this list of conditions
		and the following disclaimer.

		Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

		Neither the name of the Open Dynamics Framework Group nor the names of its contributors may
		be used to endorse or promote products derived from this software without specific prior written permission.

		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,
		INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
		DISCLAIMED. IN NO EVENT SHALL THE INTEL OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
		EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
		IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
		THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>

#include "inparser.h"

#ifdef PLAYSTATION3
#define stricmp(x, y) strcasecmp((x), (y))

#include <ctype.h> // for tolower()

char *strlwr(char *string)
{
	int length = strlen(string);

	for(int i = 0; i < length; ++i)
	{
		string[i] = static_cast<char>(toupper(string[i]));
	}
}

#endif

static char ToLower(char c)
{
	if ( c >= 'A' && c <= 'Z' ) c+=32;
	return c;
}

char *stristr(const char *str,const char *key)       // case insensitive str str
{
	assert( strlen(str) < 2048 );
	assert( strlen(key) < 2048 );
	char istr[2048];
	char ikey[2048];
	strcpy(istr,str);
	strcpy(ikey,key);
	strlwr(istr);
	strlwr(ikey);

	char *foo = strstr(istr,ikey);
	if ( foo )
	{
		unsigned int loc = (unsigned int)(foo - istr);
		foo = (char *)str+loc;
	}

	return foo;
}

bool        isstristr(const char *str,const char *key)     // bool true/false based on case insenstive strstr
{
	bool ret = false;
	const char *found = strstr(str,key);
	if ( found ) ret = true;
	return ret;
}

unsigned int GetHex(unsigned char c)
{
	unsigned int v = 0;
	c = ToLower(c);
	if ( c >= '0' && c <= '9' )
		v = c-'0';
	else
	{
		if ( c >= 'a' && c <= 'f' )
		{
			v = 10 + c-'a';
		}
	}
	return v;
}

unsigned char GetHEX1(const char *foo,const char **endptr)
{
	unsigned int ret = 0;

	ret = (GetHex(foo[0])<<4) | GetHex(foo[1]);

	if ( endptr )
	{
		*endptr = foo+2;
	}

	return (unsigned char) ret;
}


unsigned short GetHEX2(const char *foo,const char **endptr)
{
	unsigned int ret = 0;

	ret = (GetHex(foo[0])<<12) | (GetHex(foo[1])<<8) | (GetHex(foo[2])<<4) | GetHex(foo[3]);

	if ( endptr )
	{
		*endptr = foo+4;
	}

	return (unsigned short) ret;
}

unsigned int GetHEX4(const char *foo,const char **endptr)
{
	unsigned int ret = 0;

	for (int i=0; i<8; i++)
	{
		ret = (ret<<4) | GetHex(foo[i]);
	}

	if ( endptr )
	{
		*endptr = foo+8;
	}

	return ret;
}

unsigned int GetHEX(const char *foo,const char **endptr)
{
	unsigned int ret = 0;

	while ( *foo )
	{
		unsigned char c = ToLower( *foo );
		unsigned int v = 0;
		if ( c >= '0' && c <= '9' )
			v = c-'0';
		else
		{
			if ( c >= 'a' && c <= 'f' )
			{
				v = 10 + c-'a';
			}
			else
				break;
		}
		ret = (ret<<4)|v;
		foo++;
	}

	if ( endptr ) *endptr = foo;

	return ret;
}


bool         IsWhitespace(char c)
{
	if ( c == ' ' || c == 9 || c == 13 || c == 10 || c == ',' ) return true;
	return false;
}


const char * SkipWhitespace(const char *str)
{
	while ( *str && IsWhitespace(*str) ) str++;
	return str;
}

#define MAXNUM 32

float        GetFloatValue(const char *str,const char **next)
{
	float ret = 0;

	if ( next ) *next = 0;

	str = SkipWhitespace(str);

	char dest[MAXNUM];
	char *dst = dest;
	const char *hex = 0;

	for (int i=0; i<(MAXNUM-1); i++)
	{
		char c = *str;
		if ( c == 0 || IsWhitespace(c) )
		{
			if ( next ) *next = str;
			break;
		}
		else if ( c == '$' )
		{
			hex = str+1;
		}
		*dst++ = ToLower(c);
		str++;
	}

	*dst = 0;

	if ( hex )
	{
		unsigned int iv = GetHEX(hex,0);
		float *v = (float *)&iv;
		ret = *v;
	}
	else if ( dest[0] == 'f' )
	{
		if ( strcmp(dest,"fltmax") == 0 || strcmp(dest,"fmax") == 0 )
		{
			ret = FLT_MAX;
		}
		else if ( strcmp(dest,"fltmin") == 0 || strcmp(dest,"fmin") == 0 )
		{
			ret = FLT_MIN;
		}
	}
	else if ( dest[0] == 't' ) // t or 'true' is treated as the value '1'.
	{
		ret = 1;
	}
	else
	{
		ret = (float)atof(dest);
	}
	return ret;
}

int          GetIntValue(const char *str,const char **next)
{
	int ret = 0;

	if ( next ) *next = 0;

	str = SkipWhitespace(str);

	char dest[MAXNUM];
	char *dst = dest;

	for (int i=0; i<(MAXNUM-1); i++)
	{
		char c = *str;
		if ( c == 0 || IsWhitespace(c) )
		{
			if ( next ) *next = str;
			break;
		}
		*dst++ = c;
		str++;
	}

	*dst = 0;

	ret = atoi(dest);

	return ret;
}


bool CharToWide(const char *source,void *dest,int maxlen)
{
	bool ret = false;

#ifdef WIN32
	ret = true;
	mbstowcs( (wchar_t *) dest, source, maxlen );
#endif

	return ret;
}

bool WideToChar(const void *source,char *dest,int maxlen)
{
	bool ret = false;

#ifdef WIN32
	ret = true;
	wcstombs(dest, (const wchar_t *) source, maxlen );
#endif


	return ret;
}



const char * GetTrueFalse(unsigned int state)
{
	if ( state ) return "true";
	return "false";
};


const char * FloatString(float v,bool binary)
{
	static char data[64*16];
	static int  index=0;

	char *ret = &data[index*64];
	index++;
	if (index == 16 ) index = 0;

	if ( v == FLT_MAX )
	{
		strcpy(ret,"fltmax");
	}
	else if ( v == FLT_MIN )
	{
		strcpy(ret,"fltmin");
	}
	else if ( v == 1 )
	{
		strcpy(ret,"1");
	}
	else if ( v == 0 )
	{
		strcpy(ret,"0");
	}
	else if ( v == - 1 )
	{
		strcpy(ret,"-1");
	}
	else
	{
		if ( binary )
		{
			unsigned int *iv = (unsigned int *) &v;
			sprintf(ret,"%.4f$%x", v, *iv );
		}
		else
		{
			sprintf(ret,"%.9f", v );
			const char *dot = strstr(ret,".");
			if ( dot )
			{
				int len = (int)strlen(ret);
				char *foo = &ret[len-1];
				while ( *foo == '0' ) foo--;
				if ( *foo == '.' )
					*foo = 0;
				else
					foo[1] = 0;
			}
		}
	}

	return ret;
}


char * NextSep(char *str,char &c)
{
	while ( *str && *str != ',' && *str != ')' )
	{
		str++;
	}
	c = *str;
	return str;
}

int GetUserArgs(const char *us,const char *key,const char **args)
{
	int ret = 0;
	static char arglist[2048];
	strcpy(arglist,us);

	char keyword[512];
	sprintf(keyword,"%s(", key );
	char *found = strstr(arglist,keyword);
	if ( found )
	{
		found = strstr(found,"(");
		found++;
		args[ret] = found;
		ret++;
		while ( 1 )
		{
			char c;
			found = NextSep(found,c);
			if ( found )
			{
				*found = 0;
				if ( c == ',' )
				{
					found++;
					args[ret] = found;
					ret++;
				}
				else
				{
					break;
				}
			}
		}
	}
	return ret;
}

bool GetUserSetting(const char *us,const char *key,int &v)
{
	bool ret = false;

	const char *argv[256];

	int argc = GetUserArgs(us,key,argv);
	if ( argc )
	{
		v = atoi( argv[0] );
		ret = true;
	}
	return ret;
}

bool GetUserSetting(const char *us,const char *key,const char * &v)
{
	bool ret = false;

	const char *argv[256];
	int argc = GetUserArgs(us,key,argv);
	if ( argc )
	{
		v = argv[0];
		ret = true;
	}
	return ret;
}

const char **  GetArgs(char *str,int &count) // destructable parser, stomps EOS markers on the input string!
{
	InPlaceParser ipp;

	return ipp.GetArglist(str,count);
}

const char * GetRootName(const char *fname)
{
	static char scratch[512];

	const char *source = fname;

	const char *start  = fname;

  while ( *source )
  {
  	if ( *source == '/' || *source == '\\' )
  	{
  		start = source+1;
  	}
  	source++;
  }

	strcpy(scratch,start);

  char *dot = strrchr( scratch, '.' );
 
  if ( dot )
  {
  	*dot = 0;
  }

	return scratch;
}

bool IsTrueFalse(const char *c)
{
	bool ret = false;

  if ( stricmp(c,"true") == 0 || stricmp(c,"1") == 0 ) ret = true;

  return ret;
}


bool IsDirectory(const char *fname,char *path,char *basename,char *postfix)
{
	bool ret = false;

	strcpy(path,fname);
	strcpy(basename,fname);

	char *foo = path;
	char *last = 0;

	while ( *foo )
	{
		if ( *foo == '\\' || *foo == '/' ) last = foo;
		foo++;
	}

	if ( last )
	{
		strcpy(basename,last+1);
		*last = 0;
		ret = true;
	}

	const char *scan = fname;

	while ( 1 )
	{
		const char *dot = strstr(scan,".");
		if ( dot == 0 )
				break;
		scan = dot+1;
	}

	strcpy(postfix,scan);
	strlwr(postfix);

	return ret;
}

bool hasSpace(const char *str) // true if the string contains a space
{
	bool ret = false;

  while ( *str )
  {
  	char c = *str++;
  	if ( c == 32 || c == 9 )
  	{
  		ret = true;
  		break;
  	}
  }
  return ret;
}
