#ifndef NX_COLLISION_CONVEXMESHINTERFACE
#define NX_COLLISION_CONVEXMESHINTERFACE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxp.h"

class Valencies;
class Vertices;
class Scene;
class SupportVertexMap;
class NPhaseContext;

struct PxHullPolygonData;

namespace NxPhysics 
	{
	class Edge8;
	//class EdgeDesc;
	}


class ConvexMeshInterface
	{
	public:
	virtual	const Point&				GetCenter()				const	= 0;
	virtual udword						GetNbVerts()			const	= 0;
	virtual	const Point*				GetVerts()				const	= 0;
	virtual	udword						GetNbPolygons()			const	= 0;
	virtual	const PxHullPolygonData&	GetPolygon(udword i)	const	= 0;
	virtual	const PxHullPolygonData*	GetPolygons()			const	= 0;
	virtual	udword						GetNbEdges()			const	= 0;
#ifdef SUPPORT_CONVEX_EDGE_DATA
	virtual	const Vertices*				GetEdgeDirections()		const	= 0;
	virtual	const NxPhysics::Edge8*		GetEdges8()				const	= 0;
	virtual	const PxEdgeDescData*		GetEdgeToTriangles()	const	= 0;
	virtual	const uword*				GetFacesByEdges16()		const	= 0;
#endif
	virtual	udword						SelectClosestPolygon(const Point& world_witness, const Matrix4x4* world=null)					const = 0;
	virtual	udword						SelectClosestEdge(const Point& world_witness, const Matrix4x4* world=null, udword* code=null)	const = 0;

	virtual	void						ProjectHull(NPhaseContext& context, float& min, float& max, const Point& dir, const Matrix4x4& world4x4/*, const SupportVertexMap* supportVertex*/) const = 0;

	virtual	void						GetLocalBoundingSphere(NxSphere& sphere)	const	= 0;
	};

#endif
