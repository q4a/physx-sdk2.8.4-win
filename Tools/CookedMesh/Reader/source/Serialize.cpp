/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Serialize.h"


void saveChunk(NxI8 a, NxI8 b, NxI8 c, NxI8 d, NxStream& stream)
	{
	stream.storeByte(a).storeByte(b).storeByte(c).storeByte(d);
	}

void readChunk(NxI8& a, NxI8& b, NxI8& c, NxI8& d, const NxStream& stream)
	{
	a = stream.readByte();
	b = stream.readByte();
	c = stream.readByte();
	d = stream.readByte();
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NxU16 readWord(bool mismatch, const NxStream& stream)
	{
	NxU16 d = stream.readWord();
	if(mismatch)	d = flip(&d);
	return d;
	}

NxU32 readDword(bool mismatch, const NxStream& stream)
	{
	NxU32 d = stream.readDword();
	if(mismatch)	d = flip(&d);
	return d;
	}

NxF32 readFloat(bool mismatch, const NxStream& stream)
	{
	NxU32 d = stream.readDword();
	if(mismatch)	d = flip(&d);
	return FR(d);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void writeWord(NxU16 value, bool mismatch, NxStream& stream)
	{
	if(mismatch)	value = flip(&value);
	stream.storeWord(value);
	}

void writeDword(NxU32 value, bool mismatch, NxStream& stream)
	{
	if(mismatch)	value = flip(&value);
	stream.storeDword(value);
	}

void writeFloat(NxF32 value, bool mismatch, NxStream& stream)
	{
	if(mismatch)	value = flip(&value);
	stream.storeFloat(value);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool readFloatBuffer(NxF32* dest, NxU32 nbFloats, bool mismatch, const NxStream& stream)
	{
	stream.readBuffer(dest, sizeof(NxF32)*nbFloats);
	if(mismatch)
		{
		for(NxU32 i=0;i<nbFloats;i++)
			{
			dest[i] = flip(&dest[i]);
			}
		}
	return true;
	}

void writeWordBuffer(const NxU16* src, NxU32 nb, bool mismatch, NxStream& stream)
	{
	while(nb--)
		{
		NxU16 w = *src++;
		if(mismatch)	w = flip(&w);
		stream.storeWord(w);
		}
	}

void writeFloatBuffer(const NxF32* src, NxU32 nb, bool mismatch, NxStream& stream)
	{
	while(nb--)
		{
		NxF32 f = *src++;
		if(mismatch)	f = flip(&f);
		stream.storeFloat(f);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool writeHeader(NxI8 a, NxI8 b, NxI8 c, NxI8 d, NxU32 version, bool mismatch, NxStream& stream)
	{
	// Store endianness
	NxI8 streamFlags = littleEndian();
	if(mismatch)	streamFlags^=1;

	// Export header
	saveChunk('N', 'X', 'S', streamFlags, stream);	// "Novodex stream" identifier
	saveChunk(a, b, c, d, stream);					// Chunk identifier
//	stream.storeDword(version);						// Version number
	writeDword(version, mismatch, stream);

	return true;
	}

bool readHeader(NxI8 hdr1, NxI8 hdr2, NxI8 hdr3, NxI8& a, NxI8& b, NxI8& c, NxI8& d, NxU32& version, bool& mismatch, const NxStream& stream)
	{
	// Import header
	NxI8 h1, h2, h3, h4;
	readChunk(h1, h2, h3, h4, stream);
	if(h1!=hdr1 || h2!=hdr2 || h3!=hdr3)
		return false;

	NxI8 fileLittleEndian = h4&1;
	mismatch = fileLittleEndian!=littleEndian();

	readChunk(a, b, c, d, stream);

	version = readDword(mismatch, stream);
	return true;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NxU32 computeMaxIndex(const NxU32* indices, NxU32 nbIndices)
	{
	NxU32 maxIndex=0;
	while(nbIndices--)
		{
		NxU32 currentIndex = *indices++;
		if(currentIndex>maxIndex)	maxIndex = currentIndex;
		}
	return maxIndex;
	}

void storeIndices(NxU32 maxIndex, NxU32 nbIndices, const NxU32* indices, NxStream& stream, bool platformMismatch)
	{
	if(maxIndex<=0xff)
		{
		for(NxU32 i=0;i<nbIndices;i++)
			stream.storeByte((NxU8)indices[i]);
		}
	else if(maxIndex<=0xffff)
		{
		for(NxU32 i=0;i<nbIndices;i++)
			writeWord(indices[i], platformMismatch, stream);
		}
	else
		{
		writeIntBuffer(indices, nbIndices, platformMismatch, stream);
		}
	}

void readIndices(NxU32 maxIndex, NxU32 nbIndices, NxU32* indices, const NxStream& stream, bool platformMismatch)
	{
	if(maxIndex<=0xff)
		{
		for(NxU32 i=0;i<nbIndices;i++)
			indices[i] = stream.readByte();
		}
	else if(maxIndex<=0xffff)
		{
		for(NxU32 i=0;i<nbIndices;i++)
			indices[i] = readWord(platformMismatch, stream);
		}
	else
		{
		readIntBuffer(indices, nbIndices, platformMismatch, stream);
		}
	}

void readIndices(NxU32 maxIndex, NxU32 nbIndices, NxU16* indices, const NxStream& stream, bool platformMismatch)
	{
	if(maxIndex<=0xff)
		{
		for(NxU32 i=0;i<nbIndices;i++)
			indices[i] = stream.readByte();
		}
	else
		{
		for(NxU32 i=0;i<nbIndices;i++)
			indices[i] = readWord(platformMismatch, stream);
		}
	}
