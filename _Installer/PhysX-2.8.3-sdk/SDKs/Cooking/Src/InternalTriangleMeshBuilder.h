#ifndef NX_COLLISION_INTERNALTRIANGLEMESHBUILDER
#define NX_COLLISION_INTERNALTRIANGLEMESHBUILDER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "InternalTriangleMesh.h"

	// PT: we can't inherit from InternalTriangleMesh for this builder since it's aggregated in other classes! And of course I
	// don't want to change that just because of stupid cooking. So here we are with this truly ugly thing below.
	class InternalTriangleMeshBuilder
	{
		public:
										InternalTriangleMeshBuilder(InternalTriangleMesh* mesh);
										~InternalTriangleMeshBuilder();

				bool					cleanMesh();
				void					remapTopology(const NxU32* order);
				bool					createOpcodeModel(NxHeightFieldAxis heightFieldVerticalAxis, NxReal heightFieldVerticalExtent);
	#ifdef SUPPORT_CONVEX_PARTS
				bool					createConvexParts();
	#endif
	#ifdef SUPPORT_EXTRA_TRIG_DATA
				void					createSharedEdgeData();
	#endif
				InternalTriangleMesh*	mesh;
	};

	void saveCollisionTree(const MeshModel* model, bool platformMismatch, NxStream& stream);

#endif
