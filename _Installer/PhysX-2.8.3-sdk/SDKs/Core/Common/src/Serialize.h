/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#ifndef NX_SERIALIZE
#define NX_SERIALIZE

	#include "Physics.h"
	#include "FPU.h"
	class NxStream;

	void	saveChunk(NxI8 a, NxI8 b, NxI8 c, NxI8 d, NxStream& stream);
	void	readChunk(NxI8& a, NxI8& b, NxI8& c, NxI8& d, const NxStream& stream);

	NxU16	readWord(bool mismatch, const NxStream& stream);
	NxU32	readDword(bool mismatch, const NxStream& stream);
	NxF32	readFloat(bool mismatch, const NxStream& stream);

	void	writeWord(NxU16 value, bool mismatch, NxStream& stream);
	void	writeDword(NxU32 value, bool mismatch, NxStream& stream);
	void	writeFloat(NxF32 value, bool mismatch, NxStream& stream);

	bool	readFloatBuffer(NxF32* dest, NxU32 nbFloats, bool mismatch, const NxStream& stream);
	void	writeWordBuffer(const NxU16* src, NxU32 nb, bool mismatch, NxStream& stream);
	void	writeFloatBuffer(const NxF32* src, NxU32 nb, bool mismatch, NxStream& stream);

	bool	writeHeader(NxI8 a, NxI8 b, NxI8 c, NxI8 d, NxU32 version, bool mismatch, NxStream& stream);
	bool	readHeader(NxI8 a, NxI8 b, NxI8 c, NxI8 d, NxU32& version, bool& mismatch, const NxStream& stream);

	NX_INLINE	NxU16 flip(const NxU16* v)
		{
		const NxU8* b = (const NxU8*)v;
		NxU16 f;
		NxU8* bf = (NxU8*)&f;
		bf[0] = b[1];
		bf[1] = b[0];
		return f;
		}

	NX_INLINE   NxU32 flip(const NxU32* v)
		{
		const NxU8* b = (const NxU8*)v;
		NxU32 f;
		NxU8* bf = (NxU8*)&f;
		bf[0] = b[3];
		bf[1] = b[2];
		bf[2] = b[1];
		bf[3] = b[0];
		return f;
		}

#if !defined(__PPCGEKKO__) & !defined(CAFE)
	// This one is to flip floats for reading
	NX_INLINE	NxF32 flip(const NxF32* v)
		{
		NxU32 d = flip((const NxU32*)v);

		return NX_FR(d);
		}
#endif 

	// This one is to flip floats for writing
	NX_INLINE	void flipForWriting(NxF32& v)
		{
		NxU32 d = flip((const NxU32*)&v);

		// MS: It is important to modify the value directly and not use a temporary variable or a return
		//     value. The reason for this is that a flipped float might have a bit pattern which indicates
		//     an invalid float. If such a float is assigned to another float, the bit pattern
		//     can change again (hell knows why; maybe to map invalid floats to a common invalid pattern?).
		//     When reading the float and flipping again, the changed bit pattern will result in a different
		//     float than the original one.
		(NxU32&)v = d;
		}

	NX_INLINE	bool	readIntBuffer(NxU32* dest, NxU32 nbInts, bool mismatch, const NxStream& stream)
		{
		return readFloatBuffer((NxF32*)dest, nbInts, mismatch, stream);
		}

	NX_INLINE	void	writeIntBuffer(const NxU32* src, NxU32 nb, bool mismatch, NxStream& stream)
		{
		writeFloatBuffer((const NxF32*)src, nb, mismatch, stream);
		}

	NxU32 computeMaxIndex(const NxU32* indices, NxU32 nbIndices);
	void storeIndices(NxU32 maxIndex, NxU32 nbIndices, const NxU32* indices, NxStream& stream, bool platformMismatch);
	void readIndices(NxU32 maxIndex, NxU32 nbIndices, NxU32* indices, const NxStream& stream, bool platformMismatch);

#endif
