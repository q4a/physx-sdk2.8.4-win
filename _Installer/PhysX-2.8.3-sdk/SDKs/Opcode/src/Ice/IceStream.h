///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a generic stream interface.
 *	\file		IceStream.h
 *	\author		Pierre Terdiman
 *	\date		September, 13, 2004
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICESTREAM_H
#define ICESTREAM_H

	class ICECORE_API Stream
	{
		public:
									Stream()				{}
		virtual						~Stream()				{}

		// Loading API
		virtual		ubyte			ReadByte()								const	= 0;
		virtual		uword			ReadWord()								const	= 0;
		virtual		udword			ReadDword()								const	= 0;
		virtual		float			ReadFloat()								const	= 0;
		virtual		double			ReadDouble()							const	= 0;
		virtual		void			ReadBuffer(void* buffer, udword size)	const	= 0;

		// Saving API
		virtual		Stream&			StoreByte(ubyte b)								= 0;
		virtual		Stream&			StoreWord(uword w)								= 0;
		virtual		Stream&			StoreDword(udword d)							= 0;
		virtual		Stream&			StoreFloat(float f)								= 0;
		virtual		Stream&			StoreDouble(double f)							= 0;
		virtual		Stream&			StoreBuffer(const void* buffer, udword size)	= 0;

		inline_		Stream&			StoreByte(sbyte b)		{ return StoreByte(ubyte(b));	}
		inline_		Stream&			StoreWord(sword w)		{ return StoreWord(uword(w));	}
		inline_		Stream&			StoreDword(sdword d)	{ return StoreDword(udword(d));	}
	};

#endif	// ICESTREAM_H
