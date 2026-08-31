#ifndef NX_PHYSICS_ACTOR
#define NX_PHYSICS_ACTOR
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Allocateable.h"
#include "NxActor.h"
#include "NvBody.h"
#include "DebugRenderable.h"
class NvScene;
class NvShape;
class NpActor;

#if NX_SUPPORT_SWEEP_API
	struct NxSweepQueryHit;
	#include "NxUserEntityReport.h"
	class ObjectCache;
#endif

/*
Actor class for simulation. API methods should not have access.
*/
class Actor// : public NxFoundation::NxAllocateable
	{
	public:
									Actor(NvScene*);
									~Actor();

				NX_BOOL				loadFromDesc_API(const NxActorDescBase&);	//called from scene to init the actor.
				void				visualize(NxFoundation::DebugRenderable& dr);
				void				prepareStaticShape2WorldsOnActor2WorldChange(const NxMat34& newActor2World);
				NxMat34				getActor2World_API()	const;
				NxVec3				getActor2WorldP()	const;
				NxQuat				getActor2WorldQ()	const;
//				const NxMat34&		getActor2WorldReference_API()	const;
	////utility functions, pulled up out of NvBody:
				NxReal				computeKineticEnergy_API() const;
				NxVec3				transformActor2WorldSpace_API(const NxVec3& v) const;
				NxVec3				transformActor2WorldSpaceDirection_API(const NxVec3& v) const;
				NxMat33 	 		getBody2WorldOrient_API()				const;	
				NxMat34 	 		getBody2World()				const;	
				void				setCMassGlobalPose(const NxMat34& newBody2World);
	static		NxVec3				invertDiagInertia(const NxVec3& m);
				void				wakeUp();
				void				forceToSleep(bool b);
				bool				isForcedToSleep();

	NX_INLINE	NpActor*			getNpActor()									const	{ return npActor;						}
	NX_INLINE	void				setNpActor(NpActor* a)									{ npActor = a;							}
	NX_INLINE	NvBody*				getBody()										const	{ return body;							}
	NX_INLINE	void				setBody_API(NvBody* b)									{ body = b;								}
	NX_INLINE	NvShape*			getShape()										const	{ return shape;							}
	NX_INLINE	NvScene*			getOwnerScene()									const	{ return ownerScene;					}
	NX_INLINE	NxReal				getDensity()									const	{ return inpDensity;					}
	NX_INLINE	const NxMat34&		getStaticActor2World()							const	{ return inpStaticActor2World;			}
	NX_INLINE	void				setStaticActor2WorldPos_API(const NxVec3& p)			{ inpStaticActor2World.t = p;			}
	NX_INLINE	void				setStaticActor2WorldOrient_API(const NxMat33& m)		{ inpStaticActor2World.M = m;			}
	NX_INLINE	NxU32				getActorPublicFlags()							const	{ return inpPublicFlags;				}	//renamed to avoid accidental confusion with body public flags during refactor.
				void				setPublicFlags_API(NxU32 f);
				void				resetUserActorPairFiltering();
	NX_INLINE	NxActorGroup		getGroup()										const	{ return inpGroup;						}
				void				setGroup_API(NxActorGroup g);
	NX_INLINE   NxDominanceGroup	getDominanceGroup()								const	{ return inpDominanceGroup;				}
				void				setDominanceGroup_API(NxDominanceGroup g);
	NX_INLINE	NxU32				getContactReportFlags()							const	{ return inpContactReportFlags; }
				void				setContactReportFlags_API(NxU32 flags);

	NX_INLINE	void*				getMirrorInfo()									const	{ return mirrorInfo;					}
	NX_INLINE	void				setMirrorInfo(void*i)									{ mirrorInfo = i;						}
				
				NxForceFieldMaterial getForceFieldMaterial()	const						{ return mForceFieldMaterial;	}
				void 				setForceFieldMaterial(NxForceFieldMaterial mat)			{ mForceFieldMaterial = mat;	}

				NxU32				massFromShapes_API(NxReal density, NxReal& massInOut, NxMat34& massPoseInOut, NxVec3& localTensorDest,NxU32 lockCOM);  // if (lockCom) then center of mass is not clobbered and tensor adjusted appropriately
				NvShape*			createShape_API(const NxShapeDesc&);
				void				releaseShape_API(NvShape&);
				//void				removeShapeFromScene_API();
				//NxU32				createBody_API(const NxBodyDesc& bdesc);

#if NX_SUPPORT_SWEEP_API
	virtual		NxU32				linearSweep(const NxVec3& motion, NxU32 flags, void* userData, NxU32 nbShapes, NxSweepQueryHit* shapes, NxUserEntityReport<NxSweepQueryHit>* callback, const NxSweepCache* sweepCache);
#endif

				NxU32				getNbShapes()		const;
				NvShape**			getShapes();		//TODO: AM: get rid of duplicate array!!
				NxShape**			getNxShapes()		const;
//	NX_INLINE	NxU32				getManagedSceneIndex()							const	{ return inpSceneIndex;			}				
	private:
				NvShape*			shapeFactory(const NxShapeDesc& desc);
				void				getDimsFromBodyInertia(const NxVec3& inertiaMoments, NxReal mass, NxVec3& dims)	const;	//this is a utility, we could expose it to the user.


				NpActor*			npActor;		//constant, associated at object creation
				NvScene*			ownerScene;		//constant, associated at object creation
				NvBody*				body;			//zero for static actors. Set in input thread. Contains state of dynamic actors.
				NvShape*			shape;			//single collision detection shape. Only valid if compound is 0. PT: great, and where is "compound" now?
				NxU32				id;				//constant: this is just like our address, but deterministic
				NxU32				inpPublicFlags;	//Combination of ::NxActorFlag flags
				NxU32				inpForcedToSleepCounter;	//if this counter is the same as the scene simulate counter, we're forced asleep. This is to avoid the hassle of having to go through all forced to sleep actors and lower their forced to sleep flag every frame.
				NxReal				inpDensity;		//so we can save it out again.
				NxActorGroup		inpGroup;		//group number of actor. Set by user.
				NxDominanceGroup	inpDominanceGroup;//dominance group number of actor. Set by user.
				NxU32				inpContactReportFlags;
				NxForceFieldMaterial mForceFieldMaterial;
//				NxU32				inpSceneIndex;	//managed scene index
				void*				mirrorInfo;		//hook for mirroring of this actor.  Used by HardwareAbstraction
	mutable	/*<-- TEMP: see comment at end of line*/	NxMat34		inpStaticActor2World;		//only used by static actors		: TEMP: to implement the deprecated getGlobalPoseReference(), this is used by dynamics as a read buffer of the body's actor2world for the user.
	};


//////Ugly things I ran out of time to refactor:
NX_INLINE NvBody* getBody(const Actor* actor)
	{
	return actor ? actor->getBody() : NULL;
	}




#endif
