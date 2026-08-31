#ifndef PXC_SHAPECONVEX_H
#define PXC_SHAPECONVEX_H

#include "px_config.h"
#include "px_convexmesh.h"
#include "px_convexhull.h"
#include "PxcVector.h"

class PxcMatrix34;
class PxcContactCacheObject;

struct PxHullPolygonData;
struct PxEdge8Data;

/*!\file
Helpers when operating on convex meshes
*/


/*!
Interface for convex or convex-imitating classes
*/
class PxcConvexInterface
{
public:
	virtual PxU32						selectClosestPolygon(const PxcVector& worldWitness, const PxcMatrix34* world=NULL)	const = 0;
	virtual PxU32						selectClosestEdge(const PxcVector& worldWitness, const PxcMatrix34* world=NULL, PxU32* code=NULL)		const = 0;
	virtual void						projectHull(const PxcVector& dir, const PxcMatrix34& world2hull, PxcContactCacheObject& cache, PxFloat& min, PxFloat& max)					const = 0;
#if PXD_TEST_CONVEX_CONVEX_OPTIM_03
	virtual void						projectHull(PxU32 nbAxes, const PxcVector* axes, const PxcMatrix34& world2hull, PxcContactCacheObject& cache, PxFloat* min, PxFloat* max)	const = 0;
#endif

	virtual void						getBounds(PxcVector& aabbMin, PxcVector& aabbMax)				const = 0;
	virtual const PxcVector&			getCenter()														const = 0;

	// Geometry access
	virtual PxU32						getNumPolygons()												const = 0;
	virtual const PxHullPolygonData&	getPolygon(PxU32 i)												const = 0;
	virtual const PxHullPolygonData*	getPolygons()													const = 0;
	virtual PxU32						getNumEdges()													const = 0;
	//virtual const PxEdge8Data*			getEdges()														const = 0;
	virtual PxU32						getNumVertices()												const = 0;
	virtual const PxcVector*			getVertices()													const = 0;
};


/*!
Convex interface for HL-shared data convexes
*/
class PxcConvexMeshHL : public PxcConvexInterface
{
public:
										PxcConvexMeshHL(PxConvexMeshData*);

	virtual PxU32						selectClosestPolygon(const PxcVector& worldWitness, const PxcMatrix34* world=NULL)	const;
	virtual PxU32						selectClosestEdge(const PxcVector& worldWitness, const PxcMatrix34* world=NULL, PxU32* code=NULL)		const;
	virtual void						projectHull(const PxcVector& dir, const PxcMatrix34& world2hull, PxcContactCacheObject& cache, PxFloat& min, PxFloat& max)					const;
#if PXD_TEST_CONVEX_CONVEX_OPTIM_03
	virtual void						projectHull(PxU32 nbAxes, const PxcVector* axes, const PxcMatrix34& world2hull, PxcContactCacheObject& cache, PxFloat* min, PxFloat* max)	const;
#endif

	virtual void						getBounds(PxcVector& aabbMin, PxcVector& aabbMax)				const;
	virtual const PxcVector&			getCenter()														const;

	// Geometry access
	virtual PxU32						getNumPolygons()												const;
	virtual const PxHullPolygonData&	getPolygon(PxU32 i)												const;
	virtual const PxHullPolygonData*	getPolygons()													const;
	virtual PxU32						getNumEdges()													const;
	//virtual const PxEdge8Data*			getEdges()														const;
	virtual PxU32						getNumVertices()												const;
	virtual const PxcVector*			getVertices()													const;


			PxConvexMeshData*			mMeshData;
};

/*!
Convex interface for box-shapes
*/
class PxcConvexBox : public PxcConvexInterface
{
public:
										PxcConvexBox(const PxcVector& halfSide);

	virtual PxU32						selectClosestPolygon(const PxcVector& worldWitness, const PxcMatrix34* world=NULL)	const;
	virtual PxU32						selectClosestEdge(const PxcVector& worldWitness, const PxcMatrix34* world=NULL, PxU32* code=NULL)		const;
	virtual void						projectHull(const PxcVector& dir, const PxcMatrix34& world2hull, PxcContactCacheObject& cache, PxFloat& min, PxFloat& max)					const;
#if PXD_TEST_CONVEX_CONVEX_OPTIM_03
	virtual void						projectHull(PxU32 nbAxes, const PxcVector* axes, const PxcMatrix34& world2hull, PxcContactCacheObject& cache, PxFloat* min, PxFloat* max)	const;
#endif

	virtual void						getBounds(PxcVector& aabbMin, PxcVector& aabbMax)				const;
	virtual const PxcVector&			getCenter()														const;

	// Geometry access
	virtual PxU32						getNumPolygons()												const;
	virtual const PxHullPolygonData&	getPolygon(PxU32 i)												const;
	virtual const PxHullPolygonData*	getPolygons()													const;
	virtual PxU32						getNumEdges()													const;
	//virtual const PxEdge8Data*			getEdges()														const;
	virtual PxU32						getNumVertices()												const;
	virtual const PxcVector*			getVertices()													const;


			const PxcVector&			mHalfSide;
	static /*const*/ PxcVector			mCenter;

			PxcVector					mVertices[8];
			PxHullPolygonData			mPolygons[6];
};


#endif
