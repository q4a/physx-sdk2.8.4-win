#ifndef TRACELINE_H
#define TRACELINE_H

#include "Shape.h"
#include "HeightFieldShape.h"
#include "ShapeInstancePair.h"
#include "ShapeInstancePairHL.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ContactTraceLineCallback
	{
	NxVec3 line;
	const Shape &otherShape;
	ShapeInstancePairHL& callback;

	NX_INLINE ContactTraceLineCallback(const NxVec3& line, const Shape& otherShape, ShapeInstancePairHL& callback)
		: line(line), otherShape(otherShape), callback(callback)
		{
		}

	NX_INLINE bool contact(const HeightFieldShape& hfShape, int edgeIndex, NxU32 row, NxU32 column, NxReal x, NxReal y, NxReal z, NxReal h)
		{
		const HeightField& hf = hfShape.getHeightFieldFast();
		NxReal dy = y - h;

		if (hf.isDeltaHeightInsideExtent(dy))
			{
			if (hfShape.isCollisionEdge(edgeIndex))
				{
				NxU32 feature = hfShape.getEdgeFeatureIndex(edgeIndex);
				NxMaterialIndex material = hf.getTriangleMaterial(feature);
				
				// Normal = (shape edge) x (height field edge)
				NxVec3 edge = hfShape.getEdgeDirection(edgeIndex);
				NxVec3 n = edge.cross(line);
				
				// make sure it is good
				/*
				NxReal err1 = NxMath::abs(n.dot(edge));
				NxReal err2 = NxMath::abs(n.dot(line));
				if (err1 + err2 > 0) 
					return true;
				*/

				// Make sure cross product is facing correctly before clipping
				if (n.y < 0) 
					{
					n = -n;
					}
				
				// I am not sure about this
				bool voronoiClipped = hfShape.clipShapeNormalToEdgeVoronoi(n, edgeIndex);
				// if (voronoiClipped)
				// 	{
				// 	return true;
				//	}
				
				if (n.y < 0) 
					{
					// degenerate case
					return true;
					}
				
				NxReal ll = n.magnitudeSquared();
				if (ll > 0) 
					{
					// normalize
					n /= NxMath::sqrt(ll);
					}
				else 
					{
					// degenerate case
					return true; 
					}
				
				// Scale delta height so it becomes the "penetration" along the normal
				dy *= n.y;
				if (hf.getVerticalExtent() > 0 || hf.getThickness() > 0)
					{
					n = -n;
					dy = -dy;
					}
				
				// compute the contact point
				NxVec3 point(hfShape.getRowScale() * (NxReal(row) + x), h, hfShape.getColumnScale() * (NxReal(column) + z));

				// add contact
				callback.contact(otherShape, hfShape,  dy, hfShape.getAbsPoseFast() * point, hfShape.getAbsPoseFast().M * n, otherShape.getMaterial(), hfShape.getMaterialIndexHighBits() | material, 0xffffffff, feature); 
				}
			}
		return true;
		}
	};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct TriggerTraceLineCallback
	{
	NxVec3 line;
	bool intersection;

	NX_INLINE TriggerTraceLineCallback(const NxVec3& line)
		: line(line), intersection(false)
		{
		}

	NX_INLINE bool contact(const HeightFieldShape& hfShape, int edgeIndex, NxU32 row, NxU32 column, NxReal x, NxReal y, NxReal z, NxReal h)
		{
		const HeightField& hf = hfShape.getHeightFieldFast();
		NxReal dy = y - h;
		if (hf.isDeltaHeightInsideExtent(dy))
			{
			NxU32 feature = hfShape.getEdgeFeatureIndex(edgeIndex);
			if ((feature != 0xffffffff) && hfShape.isCollisionEdge(edgeIndex))
				{
				intersection = true;
				return false;
				}
			}
		return true;
		}
	};

#endif
