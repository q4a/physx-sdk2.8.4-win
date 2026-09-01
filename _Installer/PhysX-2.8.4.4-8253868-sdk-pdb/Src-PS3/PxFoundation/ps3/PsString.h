#ifndef PX_STRING_H
#define PX_STRING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "PxMath.h"

#ifndef __CELLOS_LV2__
	#error "This file should only be included by ps3 builds!!"
#endif

#include <math.h>

#define __CELLOS_LV2__INLINE inline __attribute__((always_inline))

namespace physx
{
	namespace string
	{
	//! \brief platform-specific absolute value
	__CELLOS_LV2__INLINE unsigned int	strlen(const char *str) { return (unsigned int)::strlen(str); };
	__CELLOS_LV2__INLINE void			strncpy(char *dest,const char *src,int maxlen) { ::strncpy(dest,src,maxlen); };
	__CELLOS_LV2__INLINE void			strcpy(char *dest,const char *src) { ::strcpy(dest,src); };
	__CELLOS_LV2__INLINE int			stricmp(const char *str, const char *str1) {return(::strcasecmp(str, str1));}
	__CELLOS_LV2__INLINE int			strnicmp(const char *str, const char *str1, size_t len) {return(::strncasecmp(str, str1, len));}

	__CELLOS_LV2__INLINE int _vsnprintf(char* dest, size_t size, const char* src, va_list arg) 
	{
		int r = ::vsnprintf( dest, size, src, arg );

		return r;
	}

	__CELLOS_LV2__INLINE int vsprintf_s(char* dest, size_t size, const char* src, va_list arg) 
	{
		int r = ::vsprintf( dest, src, arg );

		return r;
	}

	inline int sprintf_s( char * _DstBuf, size_t _DstSize, const char * _Format, ...)
	{
		if ( _DstBuf == NULL || _Format == NULL )
		{
			return -1;
		}

		va_list arg;
		va_start( arg, _Format );
		int r = ::vsprintf( _DstBuf, _Format, arg );
		va_end(arg);

		return r;
	}

	__CELLOS_LV2__INLINE void strlwr(char* str)
	{
		int i = 0;
		while (str[i])
		{
			if ((str[i]>=65)&&(str[i]<=90))
			{
				str[i]=str[i]+32;
			}
				
			i++;
		}
	}

	__CELLOS_LV2__INLINE void strupr(char* str)
	{
		int i = 0;
		while (str[i])
		{
			str[i] = toupper(str[i]);
			i++;
		}
	}

	__CELLOS_LV2__INLINE int strncpy_s( char *strDest,size_t sizeInBytes,const char *strSource,size_t count)
	{
		/*
			From Microsoft help: strncpy_s will try to copy the first d characters of strSource to strDest,
			where d is the lesser of count and the length of strSource. If those d characters will fit within 
			strDest (whose size is given as sizeInBytes) and still leave room for a null terminator, 
			then those characters are copied and a terminating null is appended; otherwise, strDest[0] is 
			set to the null character 
		*/
		size_t d;

		if (	strDest		== NULL ||
				strSource	== NULL ||
				sizeInBytes == 0	)
		{
			return -1;
		}
		d = physx::PxMin<size_t>(count, strlen(strSource));

		if ( sizeInBytes <= d )
		{
			strDest[0] = 0;
			return -1;
		}

		::memcpy( strDest, strSource, d );
		strDest[d] = '\0';

		return 0;
	}

	__CELLOS_LV2__INLINE int strcpy_s(char* dest, size_t size, const char* src)
	{
		/*
			The strcpy_s function copies the contents in the address of src, including the 
			terminating null character, to the location specified by dest. 
			The destination string must be large enough to hold the source string, including the 
			terminating null character.
		*/
		size_t d;

		if (	dest		== NULL ||
				src			== NULL ||
				size		== 0	)
		{
			return -1;
		}

		d = strlen(src);

		if ( size <= d )
		{
			dest[0] = 0;
			return -1;
		}

		::memcpy( dest, src, d );
		dest[d] = '\0';

		return 0;
	}

	/*
	 The strcat_s function appends strSource to strDestination and terminates the resulting string with a 
	 null character. The initial character of strSource overwrites the terminating null character of 
	 strDestination. The behavior of strcat_s is undefined if the source and destination strings overlap.
	 Note that the second parameter is the total size of the buffer, not the remaining size
	*/
	__CELLOS_LV2__INLINE int strcat_s(char* dest, size_t size, const char* src)
	{
		size_t d, destStringLen, srcStringLen;

		if (	dest		== NULL ||
				src			== NULL ||
				size		== 0	)
		{
			return -1;
		}

		destStringLen	= strlen(dest);
		srcStringLen	= strlen(src);
		d				= srcStringLen + destStringLen;

		if ( size <= d )
		{
			return -1;
		}

		::memcpy( &dest[destStringLen], src, srcStringLen );
		dest[d] = '\0';

		return 0;
	}

	/*
		strncat_s tries to append the first D characters of strSource to the end of strDest, 
		where D is the lesser of count and the length of strSource. If appending those D characters 
		will fit within strDest (whose size is given as numberOfElements) and still leave room 
		for a null terminator, then those characters are appended, starting at the original terminating 
		null of strDest, and a new terminating null is appended; otherwise, strDest[0] is set to 
		the null character 
	*/
	__CELLOS_LV2__INLINE int strncat_s(char *dest, size_t size, const char *src, size_t count) 
	{
		size_t d, destStringLen, srcStringLen;

		if (	dest		== NULL ||
				src			== NULL ||
				size		== 0	)
		{
			return -1;
		}

		srcStringLen	= physx::PxMin<size_t>(count, strlen(src));
		destStringLen	= strlen(dest);
		d				= srcStringLen + destStringLen;

		if ( size <= d )
		{
			dest[0] = '\0';
			return -1;
		}

		::memcpy( &dest[destStringLen], src, srcStringLen );
		dest[d] = '\0';

		return 0;
	}

	__CELLOS_LV2__INLINE int StringCchPrintfA( char * _DstBuf, size_t _DstSize, const char * _Format, ...)
	{
		if ( _DstBuf == NULL || _Format == NULL )
		{
			return -1;
		}

		va_list arg;
		va_start( arg, _Format );
		int r = ::vsprintf( _DstBuf, _Format, arg );
		va_end(arg);

		return r;
	}
}
}

#endif

