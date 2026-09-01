/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#include "NxParamUtils.h"
#include "NxParameterized.h"

namespace NxParameterized
{

#pragma warning(push)
#pragma warning(disable: 4996)


#define MAX_SEARCH_NAME 1024
#define MAX_SEARCH_NAMES 32

struct ParameterFind
{
	ParameterFind(const NxParameterized::Interface &iface) : mNameIndex(0), mNameCount(0), mResult(iface), mInterface(NULL), mError(false)
	{

	}
	bool isDigit(char c) const
	{
		return (c>='0' && c <='9');
	}

	// Scan's the search name, parses the array indexes.
	bool setSearchName(const char *str)
	{
		bool ret = true;

		mNameIndex = 0;
		mNameCount = 0;
		mInterface = 0;
		mError = false;
		char *dest = mSearchName;
		char *stop = &mSearchName[MAX_SEARCH_NAME-1];

		char *head = mSearchName;
		mArrayIndex[0] = -1;

		while ( *str && dest < stop )
		{
			if ( *str == '[' )
			{
				*dest++ = 0;
				*str++;
				if ( isDigit(*str) )
				{
					physx::PxU32 v = 0;
					while ( isDigit(*str) )
					{
						physx::PxU32 iv = *str-'0';
						v = v*10+iv;
						str++;
					}
					if ( *str == ']' )
					{
						mArrayIndex[mNameCount] = v;
					}
					else
					{
						ret = false;
						break;
					}
				}
				else
				{
					ret = false;
					break;
				}
			}
			else
			{
				if ( *str == '.' )
				{
					if ( mNameCount < MAX_SEARCH_NAMES )
					{
						mSearchNames[mNameCount] = head;
						mNameCount++;
						if ( mNameCount < MAX_SEARCH_NAMES )
						{
							mArrayIndex[mNameCount] = -1;
						}
						*dest++ = 0;
						*str++;
						head = dest;
					}
					else
					{
						ret = false;
						break;
					}
				}
				else
				{
					*dest++ = *str++;
				}
			}
		}

		*dest = 0;

		if ( head && *head )
		{
			if ( mNameCount < MAX_SEARCH_NAMES )
			{
				mSearchNames[mNameCount] = head;
				mNameCount++;
				*dest++ = 0;
				*str++;
				head = dest;
			}
			else
			{
				ret = false;
			}
		}

		return ret;
	}

	bool done(void) const
	{
		bool ret = false;
		if ( mInterface || mError ) ret = true;
		return ret;
	}

	physx::PxI32 getArrayIndex(void)
	{
		return mArrayIndex[mNameIndex];
	}

	bool nameMatch(const char *longName) const
	{
		bool ret = true;

		if (longName && strlen(longName))
		{
#if defined(PX_GNUC)
			if ( strcasecmp(longName,mSearchNames[mNameIndex]) == 0 )
#else
			if ( stricmp(longName,mSearchNames[mNameIndex]) == 0 )
#endif				
			{
				ret = true;
			}
			else
			{
				ret = false;
			}
		}

		return ret;
	}

	bool isComplete(void) const
	{
		return (mNameIndex+1) == mNameCount;
	}

	bool pushNameMatch(void)
	{
		bool ret = false;

		if ( (mNameIndex+1) < mNameCount )
		{
			mNameIndex++;
			ret = true;
		}
		return ret;
	}

	void popNameMatch(void)
	{
		if ( mNameIndex )
		{
			mNameIndex--;
		}
	}

	physx::PxU32						mNameIndex;
	physx::PxU32				    	mNameCount;
	Handle 								mResult;
	const NxParameterized::Interface	*mInterface;
	bool								mError;
	char								mSearchName[MAX_SEARCH_NAME];
	char				  				*mSearchNames[MAX_SEARCH_NAMES];
	physx::PxI32						mArrayIndex[MAX_SEARCH_NAMES];
};


PX_INLINE void findParameter(const NxParameterized::Interface &obj,
							 Handle &handle,
							 ParameterFind &pf,
							 bool fromArray=false)
{
	if ( pf.done() ) return;

	if ( handle.numIndexes() < 1 )
	{
		obj.getParameterHandle("",handle);
	}

	const Definition *pd = handle.parameterDefinition();
	const char *name = pd->name();
	DataType t = pd->type();

	if ( fromArray || pf.nameMatch(name) )
	{
		NxParameterized::Interface *paramPtr = 0;
		if ( t == TYPE_REF )
		{
			handle.getParamRef(paramPtr);
			if ( paramPtr  )
			{
				if ( pf.pushNameMatch() )
				{
					Handle newHandle(*paramPtr);
					paramPtr->getParameterHandle("", newHandle);
					findParameter(*paramPtr,newHandle,pf);
					pf.popNameMatch();
				}
			}
			if ( pf.isComplete() )
			{
				pf.mInterface = &obj;
				pf.mResult    = handle;
			}
		}
		if ( t == TYPE_STRUCT )
		{
			bool pushed = false;
			if ( strlen(name) )
			{
				pf.pushNameMatch();
				pushed = true;
			}
			physx::PxU32 count = (physx::PxU32)pd->numChildren();
			for (physx::PxU32 i=0; i<count; i++)
			{
				handle.set(i);
				findParameter(obj,handle,pf);
				handle.popIndex();
				if ( pf.done() ) break;
			}
			if ( pushed )
			{
				pf.popNameMatch();
			}
		}
		else if ( t == TYPE_ARRAY )
		{
			physx::PxI32 arraySize;
			handle.getArraySize(arraySize);
			physx::PxI32 arrayIndex = pf.getArrayIndex();
			if ( arrayIndex == -1 && pf.isComplete() )
			{
				pf.mInterface = &obj;
				pf.mResult    = handle;
			}
			else if ( arrayIndex >= 0 && arrayIndex < arraySize )
			{
				handle.set(arrayIndex);
				if ( pf.isComplete() )
				{
					pf.mInterface = &obj;
					pf.mResult    = handle;
				}
				else
				{
					findParameter(obj,handle,pf,true);
				}
				handle.popIndex();
			}
			else
			{

				pf.mError = true;
			}
		}
		else if ( pf.isComplete() )
		{
			pf.mInterface = &obj;
			pf.mResult    = handle;
		}
	}
}

PX_INLINE NxParameterized::Interface * findParam(const Interface &i,const char *long_name, Handle &result)
{
	NxParameterized::Interface *ret = 0;
	result.setInterface((NxParameterized::Interface *)0);

	ParameterFind pf(i);
	if ( pf.setSearchName(long_name) )
	{
		Handle handle(i);
		findParameter(i,handle,pf);
		result 	= pf.mResult;
		ret 	= (NxParameterized::Interface *)pf.mInterface;
	}
	return ret;
}


struct ParameterList
{
	ParameterList(const NxParameterized::Interface &iface,const char *className,const char *paramName,bool recursive,bool classesOnly,NxParameterized::Traits *traits) 
		: mNameIndex(0), 
		mResult(iface), 
		mInterface(NULL), 
		mClassName(className), 
		mParamName(paramName), 
		mRecursive(recursive), 
		mClassesOnly(classesOnly), 
		mTraits(traits), 
		mResultCount(0), 
		mMaxResults(0), 
		mResults(NULL)
	{

	}

	~ParameterList(void)
	{
	}

	bool nameMatch(const Interface *iface,const char * name,physx::PxI32 arrayIndex,DataType type,Handle &handle)
	{
		size_t slen = strlen(name);

		if ( mClassesOnly )
		{
			if ( slen > 0 ) return true;
		}
		else
		{
			if ( slen == 0 ) return true;
		}

		bool match = true;
		if ( mClassName )
		{
			const char *cname = iface->className();
			if ( strcmp(cname,mClassName) != 0 || strlen(name) )
			{
				match = false;
			}
		}
		if ( mParamName ) // if we specified a parameter name, than only include exact matches of this parameter name.
		{
			if ( strcmp(mParamName,name) != 0 )
			{
				match = false;
			}

		}
		if ( match )
		{
			// ok..let's build the long name...
			const char *longName = NULL;
			char scratch[1024];
			scratch[0] = 0;

			if ( slen > 0 )
			{

				for (physx::PxU32 i=0; i<mNameIndex; i++)
				{
					local_strcat_s(scratch, sizeof(scratch), mSearchName[i]);
					if ( mArrayIndex[i] > 0 )
					{
						char arrayIndexStr[32];
						arrayIndexStr[0] = 0;
						local_strcat_s(arrayIndexStr, sizeof(arrayIndexStr), "[0]");
						//sprintf_s(arrayIndexStr, sizeof(arrayIndexStr), "[%d]", 0); //mArrayIndex[i]);
						local_strcat_s(scratch, sizeof(scratch), arrayIndexStr);
					}
					local_strcat_s(scratch, sizeof(scratch),".");
				}

				local_strcat_s(scratch, sizeof(scratch),name);
				physx::PxU32 len = (physx::PxU32)strlen(scratch);
				char *temp = (char *)mTraits->alloc(len+1);
				temp[0] = 0;
				local_strcat_s(temp, len+1, scratch);
				longName = temp;
				if ( type == TYPE_ARRAY )
				{ 
					handle.getArraySize(arrayIndex);
				}
			}

			ParamResult pr(name,longName,iface->className(),iface->name(), handle, arrayIndex, type );

			if ( mResultCount >= mMaxResults )
			{
				mMaxResults = mMaxResults ? mMaxResults*2 : 32;
				ParamResult *results = (ParamResult *)mTraits->alloc(sizeof(ParamResult)*mMaxResults);
				if ( mResults )
				{
					for (physx::PxU32 i=0; i<mResultCount; i++)
					{
						results[i] = mResults[i];
					}
				}
				mResults = results;
			}
			mResults[mResultCount] = pr;
			mResultCount++;
		}

		return true; // always matches....
	}

	bool pushName(const char *name,physx::PxI32 arrayIndex)
	{
		mSearchName[mNameIndex] = name;
		mArrayIndex[mNameIndex] = arrayIndex;
		mNameIndex++;
		return true;
	}

	void popNameMatch(void)
	{
		if ( mNameIndex )
		{
			mNameIndex--;
		}
	}

	bool isRecursive(void) const { return mRecursive; };

	physx::PxU32 getResultCount(void) const { return mResultCount; };
	ParamResult * getResults(void) const { return mResults; };

	physx::PxU32				mNameIndex;
	Handle 						mResult;
	const NxParameterized::Interface *mInterface;
	const char *				mClassName;
	const char *				mParamName;
	bool						mRecursive;
	bool						mClassesOnly;
	NxParameterized::Traits		*mTraits;
	physx::PxU32				mResultCount;
	physx::PxU32				mMaxResults;
	ParamResult					*mResults;
	const char					*mSearchName[MAX_SEARCH_NAME];
	physx::PxI32				mArrayIndex[MAX_SEARCH_NAME];

};


PX_INLINE void listParameters(const NxParameterized::Interface &obj,
			   		Handle &handle,
			   		ParameterList &pf,
					physx::PxI32 parentArraySize)
{
	if ( handle.numIndexes() < 1 )
	{
		obj.getParameterHandle("",handle);
	}

	const Definition *pd = handle.parameterDefinition();
	const char *name = pd->name();
	DataType t = pd->type();

	if ( pf.nameMatch(&obj,name,parentArraySize,t,handle) )
	{
		NxParameterized::Interface *paramPtr = 0;
		if ( t == TYPE_REF )
		{
			handle.getParamRef(paramPtr);
		}
		if ( t == TYPE_STRUCT )
		{
			bool pushed=false;
			if ( strlen(name) )
			{
				pf.pushName(name,parentArraySize);
				pushed = true;
			}
       		physx::PxU32 count = (physx::PxU32)pd->numChildren();
       		for (physx::PxU32 i=0; i<count; i++)
       		{
       			handle.set(i);
       			listParameters(obj,handle,pf,0);
       			handle.popIndex();
			}
			if ( pushed )
			{
				pf.popNameMatch();
			}
		}
		else if ( t == TYPE_ARRAY )
		{
   			physx::PxI32 arraySize;
   			handle.getArraySize(arraySize);
			if ( arraySize > 0 )
			{
				for (physx::PxI32 i=0; i<arraySize; i++)
				{
					handle.set(i);
					listParameters(obj,handle,pf,arraySize);
					const Definition *pd = handle.parameterDefinition();
					DataType t = pd->type();
					handle.popIndex();
					if ( !(t == TYPE_STRUCT || t == TYPE_ARRAY || t == TYPE_REF) )
					{
						break;
					}
				}
			}
		}
		else if ( t == TYPE_REF && pf.isRecursive() )
		{
			if ( paramPtr && pd->isIncludedRef() )
			{
				if ( pf.pushName(name,parentArraySize) )
				{
     				Handle newHandle(*paramPtr);
     				paramPtr->getParameterHandle("", newHandle);
     				listParameters(*paramPtr,newHandle,pf,0);
     				pf.popNameMatch();
     			}
			}
		}
	}
}


/**
\brief Gets every parameter in an NxParameterized Interface class
\note The return pointer is allocated by the NxParameterized Traits class and should be freed by calling releaseParamList
*/
PX_INLINE const ParamResult *	getParamList(const Interface &i,const char *className,const char *paramName,physx::PxU32 &count,bool recursive,bool classesOnly,NxParameterized::Traits *traits)
{

	PX_FORCE_PARAMETER_REFERENCE(className);

	ParameterList pl(i,className,paramName,recursive,classesOnly,traits);

   	Handle handle(i);
	listParameters(i,handle,pl,0);

	count = pl.getResultCount();

	return pl.getResults();
}

PX_INLINE void	releaseParamList(physx::PxU32 resultCount,const ParamResult *results,NxParameterized::Traits *traits)
{
	if ( results )
	{
		for (physx::PxU32 i=0; i<resultCount; i++)
		{
			const ParamResult &r = results[i];
			if ( r.mLongName )
			{
				traits->free( (void *)r.mLongName );
			}
		}
		traits->free((void *)results);
	}
}

PX_INLINE void getNamedReferences(const NxParameterized::Interface &obj,
								  Handle &handle,
								  physx::PxU32 &refCount,
								  NamedReferenceInterface &namedReference,
								  bool isRecursive)
{
	if ( handle.numIndexes() < 1 )
	{
		obj.getParameterHandle("",handle);
	}
	const Definition *pd = handle.parameterDefinition();
	DataType t = pd->type();
	{
		NxParameterized::Interface *paramPtr = 0;
		if ( t == TYPE_REF )
		{
			handle.getParamRef(paramPtr);
			if ( paramPtr )
			{
				if ( !pd->isIncludedRef() )
				{
					// ok, it is a named reference!
					if ( paramPtr->name() )
					{
						const char *remap = namedReference.namedReferenceCallback(paramPtr->className(),paramPtr->name(),handle);
						if ( remap )
						{
							paramPtr->setName(remap);
							refCount++;
						}
					}
				}
				else if ( isRecursive )
				{
					Handle newHandle(*paramPtr);
					paramPtr->getParameterHandle("", newHandle);
					getNamedReferences(*paramPtr,newHandle,refCount,namedReference,isRecursive);
				}
			}
		}
		else if ( t == TYPE_STRUCT )
		{
			physx::PxU32 count = (physx::PxU32)pd->numChildren();
			for (physx::PxU32 i=0; i<count; i++)
			{
				handle.set(i);
				getNamedReferences(obj,handle,refCount,namedReference,isRecursive);
				handle.popIndex();
			}
		}
		else if ( t == TYPE_ARRAY )
		{
			physx::PxI32 arraySize;
			handle.getArraySize(arraySize);
			if ( arraySize > 0 )
			{
				bool scan = false;
				handle.set(0);
				const Definition *pd = handle.parameterDefinition();
				if ( pd->type() == TYPE_ARRAY || pd->type() == TYPE_REF || pd->type() == TYPE_STRUCT )
				{
					scan = true;
				}
				handle.popIndex();
				if ( scan )
				{
					for (physx::PxI32 i=0; i<arraySize; i++)
					{
						handle.set(i);
						getNamedReferences(obj,handle,refCount,namedReference,isRecursive);
						handle.popIndex();
					}
				}
			}
		}
	}
}

PX_INLINE physx::PxU32 getNamedReferences(const Interface &i,
										  NamedReferenceInterface &namedReference,
										  bool recursive)
{
	Handle handle(i);
	physx::PxU32 refCount=0;
	getNamedReferences(i,handle,refCount,namedReference,recursive);
	return refCount;
}


// bool
PX_INLINE bool getParamBool(const Interface &pm, const char *name, bool &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamBool(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamBool(Interface &pm, const char *name, bool value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamBool(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// string
PX_INLINE bool getParamString(const Interface &pm, const char *name, const char *&value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamString(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamString(Interface &pm, const char *name, const char *value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamString(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// enum
PX_INLINE bool getParamEnum(const Interface &pm, const char *name,  const char *&value)
{																   
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;   
	NxParameterized::Handle handle(pm);							   
	NxParameterized::Interface *iface = findParam(pm,name,handle);   
	if ( iface )												   
	{															   
		ret = handle.getParamEnum(value);						   
	}															   
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);				   
	return (ret == NxParameterized::ERROR_NONE);													   
}																   
																   
PX_INLINE bool setParamEnum(Interface &pm, const char *name,  const char *value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamEnum(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// reference
PX_INLINE bool getParamRef(const Interface &pm, const char *name, NxParameterized::Interface *&value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamRef(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamRef(Interface &pm, const char *name,  NxParameterized::Interface *value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamRef(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// I8
PX_INLINE bool getParamI8(const Interface &pm, const char *name, physx::PxI8 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamI8(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamI8(Interface &pm, const char *name, physx::PxI8 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamI8(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// I16
PX_INLINE bool getParamI16(const Interface &pm, const char *name, physx::PxI16 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamI16(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamI16(Interface &pm, const char *name, physx::PxI16 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamI16(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// I32
PX_INLINE bool getParamI32(const Interface &pm, const char *name, physx::PxI32 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamI32(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamI32(Interface &pm, const char *name, physx::PxI32 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamI32(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// I64
PX_INLINE bool getParamI64(const Interface &pm, const char *name, physx::PxI64 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamI64(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamI64(Interface &pm, const char *name, physx::PxI64 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamI64(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// U8
PX_INLINE bool getParamU8(const Interface &pm, const char *name, physx::PxU8 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamU8(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamU8(Interface &pm, const char *name, physx::PxU8 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamU8(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// U16
PX_INLINE bool getParamU16(const Interface &pm, const char *name, physx::PxU16 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamU16(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamU16(Interface &pm, const char *name, physx::PxU16 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamU16(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// U32
PX_INLINE bool getParamU32(const Interface &pm, const char *name, physx::PxU32 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamU32(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamU32(Interface &pm, const char *name, physx::PxU32 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamU32(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// U64
PX_INLINE bool getParamU64(const Interface &pm, const char *name, physx::PxU64 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamU64(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamU64(Interface &pm, const char *name, physx::PxU64 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamU64(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// F32
PX_INLINE bool getParamF32(const Interface &pm, const char *name, physx::PxF32 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamF32(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamF32(Interface &pm, const char *name, physx::PxF32 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamF32(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// F64
PX_INLINE bool getParamF64(const Interface &pm, const char *name, physx::PxF64 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamF64(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamF64(Interface &pm, const char *name, physx::PxF64 value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamF64(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// Vec2
PX_INLINE bool getParamVec2(const Interface &pm, const char *name, physx::PxVec2 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamVec2(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamVec2(Interface &pm, const char *name, const physx::PxVec2 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamVec2(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// Vec3
PX_INLINE bool getParamVec3(const Interface &pm, const char *name, physx::PxVec3 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamVec3(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamVec3(Interface &pm, const char *name, const physx::PxVec3 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamVec3(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// Vec4
PX_INLINE bool getParamVec4(const Interface &pm, const char *name, physx::PxVec4 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamVec4(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamVec4(Interface &pm, const char *name, const physx::PxVec4 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamVec4(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// Quat
PX_INLINE bool getParamQuat(const Interface &pm, const char *name, physx::PxQuat &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamQuat(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamQuat(Interface &pm, const char *name, const physx::PxQuat &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamQuat(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// Bounds3
PX_INLINE bool getParamBounds3(const Interface &pm, const char *name, physx::PxBounds3 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamBounds3(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamBounds3(Interface &pm, const char *name, const physx::PxBounds3 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamBounds3(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// Mat33
PX_INLINE bool getParamMat33(const Interface &pm, const char *name, physx::PxMat33 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamMat33(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamMat33(Interface &pm, const char *name, const physx::PxMat33 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamMat33(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// Mat34
PX_INLINE bool getParamMat34(const Interface &pm, const char *name, physx::PxMat44 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamMat34(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamMat34(Interface &pm, const char *name, const physx::PxMat44 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamMat34(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

// Mat44
PX_INLINE bool getParamMat44(const Interface &pm, const char *name, physx::PxMat44 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getParamMat44(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool setParamMat44(Interface &pm, const char *name, const physx::PxMat44 &value)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.setParamMat44(value);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

PX_INLINE bool getParamArraySize(const Interface &pm, const char *name, physx::PxI32 &arraySize)
{
	ErrorType ret = NxParameterized::ERROR_INVALID_PARAMETER_NAME;
	NxParameterized::Handle handle(pm);
	NxParameterized::Interface *iface = findParam(pm,name,handle);
	if ( iface )
	{
		ret = handle.getArraySize(arraySize);
	}
	PX_ASSERT(ret == NxParameterized::ERROR_NONE);
	return (ret == NxParameterized::ERROR_NONE);
}

#pragma warning(pop)

};
