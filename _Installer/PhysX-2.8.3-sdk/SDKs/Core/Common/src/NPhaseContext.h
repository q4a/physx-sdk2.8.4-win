#ifndef __NPHASE_CONTEXT__
#define __NPHASE_CONTEXT__

#include "Opcode.h"
#include "InternalTriangleMesh.h"
#include "IceSupport.h"
#include "SeparatingAxes.h"
#include "TriggerPairs.h"
#include "NxThread.h"
#include "NxInterlocked.h"


/*
Description: NPhaseContext represents the state necassery to processes shape instance pairs into the 
ShapeInstancePairs. There is one of these per thread, including a default one for the main thread.

The context for the main thread can share memory with the broadphase(TODO) so care should be taken with the values
placed in the vertex tags(which are not cleared after/before the broadpahse).

Contains: vertex/face tags, colliders and trigger pairs.
*/
class NPhaseContext : public NxFoundation::NxAllocateable, public NxInterlocked::SListEntry
	{

	public:

		NPhaseContext(Scene* own);
		virtual ~NPhaseContext();

		NX_INLINE NX_BOOL tagVertex(NxU32 vertexIndex, NxU32 timeStamp) const //returns true if it was not yet tagged.
			{
			NX_ASSERT(vertexIndex<maxNbStamps);

			if(vertexTagStamps[vertexIndex]==timeStamp)	return NX_FALSE;	// Already processed
			vertexTagStamps[vertexIndex]=timeStamp;		return NX_TRUE;
			}

		NxU32 getVertexTimeStamp()
			{
			// Make sure stamps have been allocated
			NX_ASSERT(vertexTagStamps);

			// First valid timestamp is maxNbStamps. We clear the array on wrapping only.
			currentTimeStamp++;
			if(!currentTimeStamp)
				{
				if(vertexTagStamps)	NxZeroMemory(vertexTagStamps, maxNbStamps*sizeof(NxU32));	// 0 is "invalid" timestamp
				currentTimeStamp = maxNbStamps;
				}
			return currentTimeStamp;
	
			}

		NX_INLINE NX_BOOL	mustProcess(NxU32 vertexIndex, NxU32 timeStamp) const //returns true if it was not yet tagged.
			{
			NX_ASSERT(vertexIndex<maxNbStamps);

			return vertexTagStamps[vertexIndex]-timeStamp;
			}

		NX_INLINE void setTagged(NxU32 vertexIndex, NxU32 timeStamp) const
            {
			NX_ASSERT(vertexIndex<maxNbStamps);

			vertexTagStamps[vertexIndex]=timeStamp;
			}

		NX_INLINE NxU32 getVertexTagStamp(NxU32 vertexIndex) const
			{
			NX_ASSERT(vertexIndex<maxNbStamps);

			return vertexTagStamps[vertexIndex];
			}

		
		NX_INLINE NxU32* getVertexTagStamps()//TODO: remove and access through safe accessor
			{
			return vertexTagStamps;
			}

		/*
		Description: grow mesh tags is called when a mesh is added to the scene, it ensure that there are 
		enough vertex tags to include all faces or verts ie must be at least MAX(mesh faces,mesh verts).
		*/
		void growMeshTags(NxU32 nbStamps);

		void growObjectTags(NxU32 nbStamps);
		
		NX_INLINE void setOwner(Scene* scene)
			{
			owner = scene;
			}

		NX_INLINE NxFoundation::NxArraySDK<ShapePair>& getTriggerPairs()
			{
			return triggerPairs;
			}
		

		Scene*						owner;

		NxU32						refCount;

		NxU32						maxNbStamps;
		NxU32*						vertexTagStamps;
		NxU32						currentTimeStamp;

		NxU32						maxNbObjects;

		Container					sharedContainer;
		size_t*						featureCacheExternal;
		NxU32*						featureCacheInternal;
 
		NxFoundation::NxArraySDK<ShapePair> triggerPairs;

		PlanesQuery					planesCollider;//con
		Opcode::PlanesCache			planesCache;
					
		SphereQuery					sphereCollider;//con
		Opcode::SphereCache			sphereCache;
					
		LSSQuery					capsuleCollider;//con
		Opcode::LSSCache			capsuleCache;

//		Opcode::AABBTreeCollider	meshCollider;//con
//		Opcode::BVTCache			meshCache;

		OBBQuery					boxCollider;//con+other
		Opcode::OBBCache			boxCache;

		AABBQuery					aabbCollider;
		Opcode::AABBCache			aabbCache;

	// Using 2nd shared container
		Opcode::PrunedObjects		_prunedObjects;
		Opcode::CulledObjects		_culledObjects;

		SeparatingAxes				SA0, SA1;		// PT: should use shared buffers at some point

		//This is used by ContactMeshHeightfield currently....
		Container					scratchContainer;

		//See growMeshTags() for details. was part of StaticPruner.

		NxU32*						mMemory;

		Opcode::PruningTemps		pruningTemps;
	};

#endif

