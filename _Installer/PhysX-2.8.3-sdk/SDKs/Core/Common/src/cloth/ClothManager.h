#ifndef NX_PHYSICS_CLOTHMANAGER
#define NX_PHYSICS_CLOTHMANAGER

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Allocateable.h"
#include "ReadWriteMutex.h"
#include "NxCloth.h"
#include "Cloth.h"
//#include "ClothHash.h"

class Scene;

/*----------------------------------------------------------------------------*/

class ClothManager : public NxFoundation::NxAllocateable
{
public:
									ClothManager(Scene* owner);
	virtual							~ClothManager();

				Cloth*				createCloth(const DeformableDesc& desc);
				void				releaseCloth(Cloth& cloth);

				void				removeShape(Shape &shape);
				void				notifyReleasePermanentlyMirroredShape(NvShape *shape);


	NX_INLINE	NxU32				getNbCloths() const	{ return mCloths.size(); }
	NX_INLINE	Cloth**				getCloths() { return (Cloth**)mCloths.begin(); }

				void				simulate(float dt);
#if XBOX_MULT_THREAD_DEFORMABLE
				void				simulate1(float dt, NxU32 clothId);	//parallel part in simulate		
				void				simulate2(float dt);									//integrateTimeStep must run form 0 to n,  werid?
#endif
				void				endSimulation();
				bool				raycast(const NxRay& worldRay, NxVec3 &hit, NxU32 &clothId, NxU32 &vertexNr);

				Scene*				getScene() { return mScene; }

				void				getMovingPoint(NxU32 clothId, NxU32 vertexNr, ClothMovingPoint &mp);
				void				getNewPosition(NxU32 clothId, NxU32 vertexNr, NxVec3 &pos);
				void				setNewPosition(NxU32 clothId, NxU32 vertexNr, const NxVec3 &pos);
				NxReal				getInverseMass(NxU32 clothId, NxU32 vertexNr);
				void				visualize(NxFoundation::DebugRenderable & dr);
				void				swapSDKBfr();

				void				updateRawBounds();


private:
	Scene* mScene;
	NxFoundation::NxArraySDK<int> mIndexOfId;
	NxFoundation::NxArraySDK<Cloth *> mCloths;
	//ClothHash mHash;
	NxFoundation::NxArraySDK<NxU32> mFreeIds;	

#ifdef __CELLOS_LV2__
	// parallel cloth solver
	void								checkAndStartCloths();
	NxFoundation::NxArraySDK<Cloth*>	readyToSend[2];
	NxFoundation::NxArraySDK<Cloth*>	readyToFinish;
	NxU32								usedSlots, slots;
	NxU32								done;
	//NxU32								clothInProgress;	// only for EMU mode
#endif
};

/*----------------------------------------------------------------------------*/

#endif
