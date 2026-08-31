#ifndef PXC_GRID_CELL_VECTOR_H
#define PXC_GRID_CELL_VECTOR_H

#include <string.h>

#include "px_config.h"
#include "PxcMath.h"
#include "PxcVector.h"



/*!
Simple integer vector in R3 with basic operations.

Used to define coordinates of a uniform grid cell.
*/

class PxcGridCellVector
{
public:
	PXD_FORCE_INLINE PxcGridCellVector()
	{
	}

	PXD_FORCE_INLINE PxcGridCellVector(const PxcGridCellVector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	PXD_FORCE_INLINE PxcGridCellVector(PxI16 _x, PxI16 _y, PxI16 _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	PXD_FORCE_INLINE bool operator==(const PxcGridCellVector& v) const
	{
		return ((x == v.x) && (y == v.y) && (z == v.z));
	}

	PXD_FORCE_INLINE bool operator!=(const PxcGridCellVector& v) const
	{
		return ((x != v.x) || (y != v.y) || (z != v.z));
	}

	PXD_FORCE_INLINE const PxcGridCellVector operator+(const PxcGridCellVector& v)
	{
		PxcGridCellVector res;
		res.x = x + v.x;
		res.y = y + v.y;
		res.z = z + v.z;
		return res;
	}

	PXD_FORCE_INLINE const PxcGridCellVector& operator+=(const PxcGridCellVector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	PXD_FORCE_INLINE const PxcGridCellVector operator-(const PxcGridCellVector& v)
	{
		PxcGridCellVector res;
		res.x = x - v.x;
		res.y = y - v.y;
		res.z = z - v.z;
		return res;
	}

	PXD_FORCE_INLINE const PxcGridCellVector& operator-=(const PxcGridCellVector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	PXD_FORCE_INLINE const PxcGridCellVector& operator=(const PxcGridCellVector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	
	PXD_FORCE_INLINE const PxcGridCellVector operator << (const PxU32 shift) const
	{
		PxcGridCellVector res;
		res.x = x << shift;
		res.y = y << shift;
		res.z = z << shift;
		return res;
	}

	//! Shift grid cell coordinates (can be used to retrieve coordinates of a coarser grid cell that contains the defined cell)
	PXD_FORCE_INLINE const PxcGridCellVector operator >> (const PxU32 shift) const
	{
		PxcGridCellVector res;
		res.x = x >> shift;
		res.y = y >> shift;
		res.z = z >> shift;
		return res;
	}

	PXD_FORCE_INLINE const PxcGridCellVector& operator <<= (const PxU32 shift)
	{
		x <<= shift;
		y <<= shift;
		z <<= shift;
		return *this;
	}

	PXD_FORCE_INLINE const PxcGridCellVector& operator >>= (const PxU32 shift)
	{
		x >>= shift;
		y >>= shift;
		z >>= shift;
		return *this;
	}

	//! Set grid cell coordinates based on a point in space and a scaling factor
	PXD_FORCE_INLINE void set(const PxcVector& realVec, PxFloat scale)
	{
		x = static_cast<PxI16>(PxcFloor(realVec.x * scale));
		y = static_cast<PxI16>(PxcFloor(realVec.y * scale));
		z = static_cast<PxI16>(PxcFloor(realVec.z * scale));
	}

	//! Set grid cell coordinates based on a point in space
	PXD_FORCE_INLINE void set(const PxcVector& realVec)
	{
		x = static_cast<PxI16>(PxcFloor(realVec.x));
		y = static_cast<PxI16>(PxcFloor(realVec.y));
		z = static_cast<PxI16>(PxcFloor(realVec.z));
	}

	PXD_FORCE_INLINE void set(PxI16 _x, PxI16 _y, PxI16 _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	PXD_FORCE_INLINE void setZero()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	PXD_FORCE_INLINE bool isZero()
	{
		return x == 0 && y == 0 && z == 0;
	}

public:

	PxI16 x;
	PxI16 y;
	PxI16 z;
};

#endif
