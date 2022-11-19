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
#include <stdarg.h>

#include "cparser.h"
#include "filesystem.h"


#define LOG_ERROR 1

#if LOG_ERROR
#include "log.h"
#endif
#include "MyGlobals.h"



//==================================================================================
//==================================================================================
enum CommandParserCommand
{
	CPC_RUN
};

//==================================================================================
//==================================================================================
class RootCommands : public CommandParserInterface
{
public:
	RootCommands(void)
	{
		AddToken("Run",CPC_RUN);
	}

	~RootCommands(void)
	{
	}

	virtual int CommandCallback(int token,int count,const char **arglist)
	{
		int ret = 0;

		switch ( token )
		{
			case CPC_RUN:
				if ( count >= 2 )
				{
					for (int i=1; i<count; i++)
					{
						int v = gAllGlobals.gCommandParser.ref()->Batch( arglist[1] );
						if ( v ) ret = v;
					}
				}
				break;
		}

		return ret;
	}

};

//==================================================================================
CommandParser::CommandParser(void)
{
	if(!gAllGlobals.gCommandParser.ref())
		gAllGlobals.gCommandParser.set(this);
	mRoot = new RootCommands;
	mLineNo = 0;
}

//==================================================================================
CommandParser::~CommandParser(void)
{
	delete mRoot;
}

//==================================================================================
void CommandParser::AddToken(const char *key, int token, CommandParserInterface *service)
{
	StringRef rkey = SGET(key);
	TokenTag ttype(token,service);

	TokenMap::iterator found;
	found = mTokens.find( rkey );
	if ( found != mTokens.end() )
	{
		printf("Attempted to add the same token (%s) twice!!\n",key);
		assert( 0 );
	}
	else
		mTokens[ rkey ] = ttype;
}

//==================================================================================
int CommandParser::GetTokenFromKey( const char *key )
{
	int ret = -1;

	StringRef rkey = SGET(key);
	TokenMap::iterator found;
	found = mTokens.find( rkey );
	if ( found != mTokens.end() )
	{
		TokenTag ttype = (*found).second;
		ret = ttype.GetLocalToken();
	}

	return ret;
}

//==================================================================================
const char *CommandParser::GetKeyFromToken( const char *prefix, int token )
{
	const char *ret = 0;

	if ( prefix )
	{
		int prefixLen = (int)strlen( prefix );

		TokenMap::iterator i;
		for ( i = mTokens.begin(); !ret && (i != mTokens.end()); ++i )
		{
			TokenTag ttype = (*i).second;
			if ( ttype.GetLocalToken() == token )
			{
				const char *value = (*i).first;
				if ( value && !strncmp( value, prefix, prefixLen ) )
				{
					ret = value;
				}
			}
		}
	}

	return ret;
}

//==================================================================================
void CommandParser::RemoveToken(const char *key)  // JWR  remove a token we don't need anymore.
{
	StringRef rkey = SGET(key);
	TokenMap::iterator found;
	found = mTokens.find( rkey );
	if ( found != mTokens.end() )
	{
		mTokens.erase( found );
	}
	else
	{
		assert( 0 );
		printf("Attempted to remove token %s that didn't exist!\n",key);
	}
}

//==================================================================================
int __cdecl CommandParser::Parse(const char *fmt, ...)
{
	char buff[16384];

	vsprintf(buff, fmt, (char *)(&fmt+1));


	int ret = 0;

	unsigned int len = (unsigned int)strlen(buff);
	if ( len )
	{
	
		InPlaceParser ipp(buff,len);
		ipp.DefaultSymbols();


		int v = ipp.Parse(this);
		if ( v ) ret = v;
	}

	return ret;
}

//==================================================================================
int CommandParser::ParseLine(int lineno,int argc,const char **argv)
{
	mLineNo = lineno;
	return CommandLine(argc,argv,true);
}

//==================================================================================
void CommandParser::DeRegister(CommandParserInterface *interface)
{
	while ( 1 )
	{
		TokenMap::iterator i;
		for (i=mTokens.begin(); i!=mTokens.end(); ++i)
		{
			CommandParserInterface *service = (*i).second.GetService();
			if ( service == interface ) break;
		}
		if ( i == mTokens.end() ) break;
		mTokens.erase(i);
	}
}

//==================================================================================
int CommandParser::Batch(const char *fname)
{
	int ok = -1;

	const char *name = fname;
	InPlaceParser ipp(name,gAllGlobals.gFileSystem.ref());
	ipp.DefaultSymbols();

	if ( ipp.HasData() )
	{
		ok = ipp.Parse(this);
	}

	return ok;
}

//==================================================================================
int CommandParser::CommandLine(int argc,const char **argv,bool fallbackok)
{
	int ret = 0;

	if ( argc )
	{
		TokenMap::iterator found;
		StringRef ref = SGET(argv[0]);
		found = mTokens.find( ref );

		if ( found == mTokens.end() )
		{
			// do case - insenstive search
			for (found=mTokens.begin(); found!=mTokens.end(); ++found)
			{
				if ( stricmp( (*found).first.Get(), ref.Get() ) == 0 ) break;
			}
		}

		if ( found != mTokens.end() )
		{
			TokenTag ttype = (*found).second;

			if ( 0 )
			{
				for (int i=0; i<argc; i++)
				{
					gAllGlobals.gLog.ref()->Display("%s ", argv[i] );
				}
				gAllGlobals.gLog.ref()->Display("\n");
			}

			CommandParserInterface *cmd = ttype.GetService();

			int v = cmd->CommandCallback( ttype.GetLocalToken(), argc, argv );

			if ( v ) 
				ret = v;
		}
		else
		{
			if ( mFallbacks.empty() || !fallbackok )
			{
				#if LOG_ERROR
				if ( gAllGlobals.gLog.ref() )
				{
//          gAllGlobals.gLog.ref()->Display("CPARSER(%s)???\n", argv[0] );
				}
				#endif
			}
			else
			{
				CommandParserInterfaceVector::iterator i;

				for (i=mFallbacks.begin(); i!=mFallbacks.end(); ++i)
				{
					CommandParserInterface *iface = (*i);
					int v = iface->CommandFallback(argc,argv);
					if ( v ) ret = v;
				}
			}
		}
	}

	return ret;
}

//==================================================================================
bool CommandParser::receiveMessage(const char *msg)
{
  Parse(msg);
  return true;
}

//==================================================================================
void CommandParser::checkMessages(void)
{
}

//==================================================================================
void CommandParserInterface::AddToken(const char *key,int token)
{
	gAllGlobals.gCommandParser.ref()->AddToken(key,token,this);
}

//==================================================================================
int CommandParserInterface::GetTokenFromKey(const char *key)
{
	return gAllGlobals.gCommandParser.ref()->GetTokenFromKey( key );
}

//==================================================================================
const char *CommandParserInterface::GetKeyFromToken( const char *prefix, int token )
{
	return gAllGlobals.gCommandParser.ref()->GetKeyFromToken( prefix, token );
}

//==================================================================================
CommandParserInterface::CommandParserInterface(void)
{
}

//==================================================================================
CommandParserInterface::~CommandParserInterface(void)
{
	gAllGlobals.gCommandParser.ref()->DeRegister(this);
}

//==================================================================================
CommandCapture::CommandCapture(int token,int count,const char **args)
{
	mToken = token;
	mCount = count;
	mArgs = (char **) malloc(sizeof(char*)*mCount);
	for (int i=0; i<count; i++)
	{
		const char *foo = args[i];
		unsigned int len = (unsigned int)strlen(foo);
		mArgs[i] = (char *) malloc(len+1);
		strcpy(mArgs[i],foo);
	}
}

//==================================================================================
CommandCapture::CommandCapture(const CommandCapture &a) // copy constructor to do a 'deep' copy.
{
	mToken = a.mToken;
	mCount = a.mCount;
	mArgs = (char **) malloc(sizeof(char*)*mCount);
	for (int i=0; i<mCount; i++)
	{
		const char *foo = a.mArgs[i];
		unsigned int len = (unsigned int)strlen(foo);
		mArgs[i] = (char *) malloc(len+1);
		strcpy(mArgs[i],foo);
	}
}

//==================================================================================
CommandCapture::~CommandCapture(void)
{
	if ( mArgs )
	{
		for (int i=0; i<mCount; i++)
		{
			char *mem = mArgs[i];
			free(mem);
		}
		free(mArgs);
	}
}

//==================================================================================
void CommandCapture::Invoke(CommandParserInterface *iface) // spool it as a callback.
{
	iface->CommandCallback(mToken,mCount,(const char **)mArgs);
}

