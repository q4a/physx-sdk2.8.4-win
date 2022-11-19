// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//							         USER STREAM
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

#ifndef STREAM_H
#define STREAM_H

#include "NxStream.h"

class UserStream : public NxStream
{
public:
								UserStream(const char* filename, bool load);
	virtual						~UserStream();

	virtual		bool			isOpen();
    virtual		void			closeStream();
	virtual		bool			advanceStream(NxU32 nbBytes);

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

				FILE*			fp;
};

#endif  // STREAM_H
