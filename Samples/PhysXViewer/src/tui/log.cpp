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
#include <malloc.h>
#include <assert.h>
#include <stdarg.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "log.h"
#include "MyGlobals.h"

#define USE_LOG 1



static  char buff[8192];

Log::Log(void)
{
	mTrapLog = 0;
	mLogIndex = 0;
	mHaveLogMessage = false;
	for (int i=0; i<MAXLOGRETAIN; i++)
	{
		char *msg = (char *) malloc(MAXLOGSTRING);
		*msg = 0;
		mLog[i] = msg;
	}
	mBuffer[0] = 0;
	mLogFrame = 0;
	mIndex = 0;
	mLogLevel = 0;
	mFph = 0;
	mBufferMessages = true;
#if USE_LOG
	mFph = fopen("log.txt","wb");
#endif
}

Log::Log(const char *fname,bool buffermessages)
{
	mTrapLog = 0;
	mLogIndex = 0;
	mHaveLogMessage = false;
	for (int i=0; i<MAXLOGRETAIN; i++)
	{
		char *msg = (char *) malloc(MAXLOGSTRING);
		*msg = 0;
		mLog[i] = msg;
	}
	mBuffer[0] = 0;
	mLogFrame = 0;
	mIndex = 0;
	mLogLevel = 0;
	mBufferMessages = buffermessages;
#if USE_LOG
	mFph = fopen(fname,"wb");
#endif
}

Log::~Log(void)
{
	for (int i=0; i<MAXLOGRETAIN; i++)
	{
		free(mLog[i]);
	}
}


void Log::Display(const char *fmt, ... )
{
	char wbuff[8192];

	vsprintf(wbuff, fmt, (char *)(&fmt+1));


	const char *source = wbuff;
	char *dest = buff;
	char p = 0;
	unsigned int len = (unsigned int)strlen(source);

	for (unsigned int i=0; i<len; i++)
	{
		char c = *source++;
		if ( c == 10 && p != 13 )
		{
			*dest++ = 13;
		}
		*dest++ = c;
		p = c;
	}
	*dest = 0;


	if ( mTrapLog )
	{
		mTrapLog->logMessage(buff);
	}

	if ( mFph )
	{
		fprintf(mFph,"%s",buff);
		fflush(mFph);
	}

#if defined(_XBOX)
	int dprintf(const char* format,...);
	dprintf(buff);
#endif


	if ( mBufferMessages )
	{
		int len = (int)strlen(mBuffer);
		assert( len < 1024 );
		char *dest = &mBuffer[len];
		char *source = buff;
		while ( *source )
		{
			if ( *source == 13 || *source == 10 )
			{
				source++;
				*dest = 0;
    		unsigned int len = (unsigned int )strlen(mBuffer);
    		if ( len )
    		{
    			if ( len > 120 )
    			{
    				len = 120;
    				mBuffer[len-1] = 0;
    			}
    			strcpy(mLog[mLogIndex],mBuffer);
					mHaveLogMessage = true;
    			mLogIndex++;
    			if ( mLogIndex == MAXLOGRETAIN ) mLogIndex = 0;
    			mBuffer[0] = 0;
    			dest = mBuffer;
    		}
  			mBuffer[0] = 0;
   			dest = mBuffer;
			}
			else
			{
				*dest++ = *source++;
			}
		}
		*dest = 0;
	}

#ifdef WIN32
//  OutputDebugStringA(buff);
#endif

#ifdef _CONSOLE
	printf(buff);
#endif

}


const char * Log::FormatNumber(int number) // JWR  format this integer into a fancy comma delimited string
{
	char * dest = &mFormat[mIndex*MAXNUMERIC];
	mIndex++;
	if ( mIndex == MAXFNUM ) mIndex = 0;

	char scratch[512];

#if defined (LINUX_GENERIC)
	snprintf(scratch, 10, "%d", number);
#else
	itoa(number,scratch,10);
#endif

	char *str = dest;
	unsigned int len = (unsigned int)strlen(scratch);
	for (unsigned int i=0; i<len; i++)
	{
		int place = (len-1)-i;
		*str++ = scratch[i];
		if ( place && (place%3) == 0 ) *str++ = ',';
	}
	*str = 0;

	return dest;
}


void Log::Flush(void)
{
	if ( mFph )
	{
		fflush(mFph);
	}
}


const char * Log::GetLogMessage(void) // spool report output messages...
{
	const char *ret = 0;

	int index = mLogIndex+1;
	if ( index >= MAXLOGRETAIN ) index = 0;

	for (int i=0; i<MAXLOGRETAIN; i++)
	{
		char *msg = mLog[index];
		if ( strlen(msg) )
		{
  		static char rbuff[256];
		  strcpy(rbuff,msg);
		  *msg = 0;
		  ret = rbuff;
		  break;
		}
		index++;
		if ( index >= MAXLOGRETAIN ) index = 0;
	}

	if ( ret == 0 )
	{
		mHaveLogMessage = false;
	}

	return ret;
}


void Log::AddBuffer(char *buffer,const char *fmt, ... )
{
	char wbuff[8192];

	vsprintf(wbuff, fmt, (char *)(&fmt+1));

	strcat(buffer,wbuff);


}

