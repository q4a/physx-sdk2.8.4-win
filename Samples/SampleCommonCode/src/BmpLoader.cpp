#ifndef __PPCGEKKO__
#include <stdio.h>
#include <stdlib.h>
#include "BmpLoader.h"
#include "MediaPath.h"

static bool isBigEndian() { int i = 1; return *((char*)&i)==0; }

static unsigned short endianSwap(unsigned short nValue)
{
   return (((nValue>> 8)) | (nValue << 8));

}

static unsigned int endianSwap(unsigned int i)
{
  unsigned char b1, b2, b3, b4;

  b1 = i & 255;
  b2 = ( i >> 8 ) & 255;
  b3 = ( i>>16 ) & 255;
  b4 = ( i>>24 ) & 255;

  return ((unsigned int)b1 << 24) + ((unsigned int)b2 << 16) + ((unsigned int)b3 << 8) + b4;
}

// -------------------------------------------------------------------

#pragma pack(1)

struct BMPHEADER {
	unsigned short		Type;
	unsigned int		Size;
	unsigned short		Reserved1;
	unsigned short		Reserved2;
	unsigned int		OffBits;
};

// Only Win3.0 BMPINFO (see later for OS/2)
struct BMPINFO {
	unsigned int		Size;
	unsigned int		Width;
	unsigned int		Height;
	unsigned short		Planes;
	unsigned short		BitCount;
	unsigned int		Compression;
	unsigned int		SizeImage;
	unsigned int		XPelsPerMeter;
	unsigned int		YPelsPerMeter;
	unsigned int		ClrUsed;
	unsigned int		ClrImportant;
};

#pragma pack()

// Compression Type
#define BI_RGB      0L
#define BI_RLE8     1L
#define BI_RLE4     2L

// -------------------------------------------------------------------
BmpLoader::BmpLoader() 
{
	mWidth = 0;
	mHeight = 0;
	mRGB = NULL;
}


// -------------------------------------------------------------------
BmpLoader::~BmpLoader() 
{
	if (mRGB) free(mRGB);
}

// -------------------------------------------------------------------
bool BmpLoader::loadBmp(const char *filename)
{
	if (mRGB) {
		free(mRGB);
		mRGB = NULL;
	}
	mWidth = 0;
	mHeight = 0;

	char buff[512];

	FILE *f = fopen(FindMediaFile(filename,buff), "rb");
	if (!f) return false;

	size_t num;
	BMPHEADER header;
	num = fread(&header, sizeof(BMPHEADER), 1, f);
	if(isBigEndian()) header.Type = endianSwap(header.Type);
	if (num != 1) { fclose(f); return false; }
	if (header.Type != 'MB') { fclose(f); return false; }

	BMPINFO info;
	num = fread(&info, sizeof(BMPINFO), 1, f);
	if (num != 1) { fclose(f); return false; }
	if(isBigEndian()) info.Size = endianSwap(info.Size);
	if(isBigEndian()) info.BitCount = endianSwap(info.BitCount);
	if(isBigEndian()) info.Compression = endianSwap(info.Compression);
	if(isBigEndian()) info.Width = endianSwap(info.Width);
	if(isBigEndian()) info.Height = endianSwap(info.Height);

	if (info.Size != sizeof(BMPINFO)) { fclose(f); return false; }
	if (info.BitCount != 24) { fclose(f); return false; }
	if (info.Compression != BI_RGB) { fclose(f); return false; }

	mWidth = info.Width;
	mHeight = info.Height;
	mRGB = (unsigned char*)malloc(mWidth * mHeight * 3);

	int lineLen = (((info.Width * (info.BitCount>>3)) + 3)>>2)<<2;
	unsigned char *line = (unsigned char *)malloc(lineLen);

	for(int i = info.Height-1; i >= 0; i--) {
		num = fread(line, lineLen, 1, f);
		if (num != 1) { fclose(f); return false; }
		unsigned char *src = line;
		unsigned char *dest = mRGB + i*info.Width*3;
		for(unsigned int j = 0; j < info.Width; j++) {
			unsigned char r,g,b;
			b = *src++; g = *src++; r = *src++;
			*dest++ = r; *dest++ = g; *dest++ = b;
		}
	}

	free(line);
	fclose(f);

	return true;
}

#endif
