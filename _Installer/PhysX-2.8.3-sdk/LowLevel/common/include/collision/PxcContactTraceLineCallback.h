#ifndef PXC_CONTACTTRACELINECALLBACK_H
#define PXC_CONTACTTRACELINECALLBACK_H

#include "PxcShapeHeightField.h"
#include "PxcContactMethod.h"
#include "PxcMatrix34.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct PxcContactTraceLineCallback
{
	PxcVector line;
	PxcContactCallback& callback;
	PxcMatrix34 transform;
	PxU32 contactCount;

	PXD_INLINE PxcContactTraceLineCallback(const PxcVector& line, PxcContactCallback& callback, PxcMatrix34 transform)
	: line(line), callback(callback), contactCount(0), transform(transform)
	{
	}

	PXD_INLINE bool contact(const PxcHeightField& hfShape, int edgeIndex, PxU32 row, PxU32 column, PxFloat x, PxFloat y, PxFloat z, PxFloat h)
	{
		//const PxcHeightField& hf = hfShape.getHeightFieldFast();
		PxFloat dy = y - h;
		if (hfShape.isDeltaHeightInsideExtent(dy))
		{
			if (hfShape.isCollisionEdge(edgeIndex))
			{
				PxU32 feature = hfShape.getEdgeFeatureIndex(edgeIndex);

				// Normal = (shape edge) x (height field edge)
				PxcVector edge = hfShape.getEdgeDirection(edgeIndex);
				PxcVector n = edge.cross(line);

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

				PxFloat ll = n.magnitudeSquared();
				if (ll > 0) 
				{
					// normalize
					n = n / PxcSqrt(ll);
				}
				else 
				{
					// degenerate case
					return true; 
				}

				// Scale delta height so it becomes the "penetration" along the normal
				dy *= n.y;
				if (hfShape.getVerticalExtent() > 0 || hfShape.getThickness() > 0)
				{
					n = -n;
					dy = -dy;
				}

				// compute the contact point
				PxcVector point;
				point.set(hfShape.getRowScale() * (PxFloat(row) + x), h, hfShape.getColumnScale() * (PxFloat(column) + z));

				// add contact
				callback.contact(transform.transform(point), transform.rotate(n), dy, 0, feature);
				
				contactCount++;
			}
		}
		return true;
	}
};

#endif
