#ifndef NX_SWEEP_CACHE_H
#define NX_SWEEP_CACHE_H

	#include "Opcode.h"
	#include "Nx.h"
	#include "Allocateable.h"
	#include "Array.h"
	#include "NxBox.h"
	#include "NxActor.h"	// for NxSweepCache
	class Shape;
	class NvScene;

	class NxVec3;
	void createOBB(NxBox& dest, const NxBox& box, const NxVec3& dir, NxF32 d);

	bool ICE_IsBoxAInsideBoxB(const NxBox& a, const NxBox& b);

	class ObjectCache : public NxSweepCache, public NxFoundation::NxAllocateable
		{
		public:
														ObjectCache(NvScene* owner);
														~ObjectCache();

		virtual		void								setVolume(const NxBox& box);

					NvScene*							ownerScene;

//					NxFoundation::NxArraySDK<Shape*>	cachedObjects;	// PT: first lame impl
//					ContainerSizeT						cachedObjects;	// We need the "ForceSize" function...
					Opcode::PrunedObjects				cachedObjects;

					NxU32								nbStatic;
					NxBox								cachedVolume;
					SignatureState						ss,sd;

//					NxBox								userVolume;
					bool								implicit;
		};

#endif
