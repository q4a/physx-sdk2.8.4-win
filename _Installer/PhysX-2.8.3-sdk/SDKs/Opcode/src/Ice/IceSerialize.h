///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains serialization utilities.
 *	\file		IceSerialize.h
 *	\author		Pierre Terdiman
 *	\date		November, 30, 2004
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICESERIALIZE_H
#define ICESERIALIZE_H

	ICECORE_API	void	WriteChunk(ubyte a, ubyte b, ubyte c, ubyte d, CustomArray& array);
	ICECORE_API	void	WriteChunk(ubyte a, ubyte b, ubyte c, ubyte d, Stream& stream);

	ICECORE_API	void	WriteWord(uword value, bool mismatch, CustomArray& array);
	ICECORE_API	void	WriteWord(uword value, bool mismatch, Stream& stream);
	ICECORE_API	void	WriteDword(udword value, bool mismatch, CustomArray& array);
	ICECORE_API	void	WriteDword(udword value, bool mismatch, Stream& stream);
	ICECORE_API	void	WriteFloat(float value, bool mismatch, CustomArray& array);
	ICECORE_API	void	WriteFloat(float value, bool mismatch, Stream& stream);

	ICECORE_API	bool	ReadWordBuffer(uword* dest, udword nb, bool mismatch, const Stream& stream);
	ICECORE_API	bool	ReadFloatBuffer(float* dest, udword nb, bool mismatch, const Stream& stream);
	ICECORE_API	void	WriteWordBuffer(const uword* src, udword nb, bool mismatch, Stream& stream);
	ICECORE_API	void	WriteFloatBuffer(const float* src, udword nb, bool mismatch, Stream& stream);

	ICECORE_API	bool	WriteHeader(ubyte a, ubyte b, ubyte c, ubyte d, udword version, bool mismatch, Stream& stream);
	ICECORE_API	bool	ReadHeader(ubyte a_, ubyte b_, ubyte c_, ubyte d_, udword& version, bool& mismatch, const Stream& stream);

	inline_ void Flip(uword& v)
	{
		ubyte* b = (ubyte*)&v;
		ubyte temp = b[0];
		b[0] = b[1];
		b[1] = temp;
	}

	inline_ void Flip(sword& v)
	{
		Flip((uword&)v);
	}

	inline_ void Flip(udword& v)
	{
		ubyte* b = (ubyte*)&v;

        ubyte temp = b[0];
		b[0] = b[3];
		b[3] = temp;
		temp = b[1];
		b[1] = b[2];
		b[2] = temp;
	}

	inline_ void Flip(sdword& v)
	{
		Flip((udword&)v);
	}

	inline_ void Flip(float& v)
	{
		Flip((udword&)v);
	}

	//byte flipping.

	template<const size_t SIZE_T> inline_ void FlipBytes(ubyte *bytes)
	{
		ubyte tmp[SIZE_T];
		memcpy(tmp, bytes, SIZE_T);
		for(udword i=0; i<SIZE_T; i++)
			bytes[i] = tmp[SIZE_T - i - 1];
	}

	template<> inline_ void FlipBytes<4>(ubyte *bytes)
	{
		Flip((udword &)(*bytes));
	}

	template<> inline_ void FlipBytes<2>(ubyte *bytes)
	{
		Flip((uword &)(*bytes));
	}

	//pointer flipping(size safe).

	template<class T> inline_ void Flip(T *&p)
	{
		FlipBytes<sizeof(T *)>((ubyte *)&p);
	}

	inline_	bool ReadDwordBuffer(udword* dest, udword nb, bool mismatch, const Stream& stream)
	{
		return ReadFloatBuffer((float*)dest, nb, mismatch, stream);
	}

	inline_	void WriteDwordBuffer(const udword* src, udword nb, bool mismatch, Stream& stream)
	{
		WriteFloatBuffer((const float*)src, nb, mismatch, stream);
	}

	udword	ComputeMaxIndex(const udword* indices, udword nbIndices);
	void	StoreIndices(udword maxIndex, udword nbIndices, const udword* indices, Stream& stream, bool platformMismatch);
	void	ReadIndices(udword maxIndex, udword nbIndices, udword* indices, const Stream& stream, bool platformMismatch);

	uword	ComputeMaxIndex(const uword* indices, udword nbIndices);
	void	StoreIndices(uword maxIndex, udword nbIndices, const uword* indices, Stream& stream, bool platformMismatch);
	void	ReadIndices(uword maxIndex, udword nbIndices, uword* indices, const Stream& stream, bool platformMismatch);

	void	StoreIndices(udword maxIndex, udword nbIndices, const udword* indices, CustomArray& array, bool platformMismatch);
	void	ReadIndices(udword maxIndex, udword nbIndices, udword* indices, const CustomArray& array, bool platformMismatch);

	inline_ void ReadChunk(ubyte& a, ubyte& b, ubyte& c, ubyte& d, const CustomArray& array)
	{
		a = array.GetByte();
		b = array.GetByte();
		c = array.GetByte();
		d = array.GetByte();
	}

	inline_ void ReadChunk(ubyte& a, ubyte& b, ubyte& c, ubyte& d, const Stream& stream)
	{
		a = stream.ReadByte();
		b = stream.ReadByte();
		c = stream.ReadByte();
		d = stream.ReadByte();
	}

	inline_ uword ReadWord(bool mismatch, const CustomArray& array)
	{
		uword d = array.GetWord();
		if(mismatch)	Flip(d);
		return d;
	}

	inline_ uword ReadWord(bool mismatch, const Stream& stream)
	{
		uword d = stream.ReadWord();
		if(mismatch)	Flip(d);
		return d;
	}

	inline_ udword ReadDword(bool mismatch, const CustomArray& array)
	{
		udword d = array.GetDword();
		if(mismatch)	Flip(d);
		return d;
	}

	inline_ udword ReadDword(bool mismatch, const Stream& stream)
	{
		udword d = stream.ReadDword();
		if(mismatch)	Flip(d);
		return d;
	}


	inline_ float ReadFloat(bool mismatch, const CustomArray& array)
	{
		udword d = array.GetDword();
		if(mismatch)	Flip(d);
		return FR(d);
	}

	inline_ float ReadFloat(bool mismatch, const Stream& stream)
	{
		udword d = stream.ReadDword();
		if(mismatch)	Flip(d);
		return FR(d);
	}

#endif // ICESERIALIZE_H
