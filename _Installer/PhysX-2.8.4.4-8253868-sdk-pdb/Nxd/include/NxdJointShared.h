#ifndef NX_DOUBLEBUFFER_NXD_JOINTSHARED
#define NX_DOUBLEBUFFER_NXD_JOINTSHARED
/*----------------------------------------------------------------------------*\
|
|					Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxJoint.h"
#include "NxJointDesc.h"
#include "NxdCasts.h"

class NxdScene;
class NxActor;

class NxdJointShared
{
public:
	NxdJointShared(NxdScene &scene);

	static NxdJointShared *			createFromDesc(NxdScene &scene, const NxJointDesc &desc);
	void destroy();

	static NxdJointShared &			getShared(NxJoint *);
	static const NxdJointShared &	getShared(const NxJoint *);

	void				getActors(NxActor** actor1, NxActor** actor2);

	void				setGlobalAnchor(const NxVec3 &vec);
	void				setGlobalAxis(const NxVec3 &vec);
	NxVec3				getGlobalAnchor()	const ;
	NxVec3				getGlobalAxis()		const;
	NxJointState		getState();
	
	void				setBreakable(NxReal maxForce, NxReal maxTorque);
	void				getBreakable(NxReal & maxForce, NxReal & maxTorque);

	void				setSolverExtrapolationFactor(NxReal solverExtrapolationFactor);
	NxReal				getSolverExtrapolationFactor() const;

	void				setUseAccelerationSpring(bool b);
	bool				getUseAccelerationSpring() const;

	NxJointType			getType() const;
	NxScene&			getScene()			const;
	void *				is(NxJointType type); 
	void				markDirty();

	void				setName(const char* name);
	const char*			getName()			const;

	void				setLimitPoint(const NxVec3 & point, bool pointIsOnActor2 = true);
	bool				getLimitPoint(NxVec3 & worldLimitPoint);

	bool				addLimitPlane(const NxVec3 & normal, const NxVec3 & pointInPlane, NxReal restitution = 0.0f);
	void				purgeLimitPlanes();
	void				resetLimitPlaneIterator();
	bool				hasMoreLimitPlanes();

	bool				getNextLimitPlane(NxVec3 & planeNormal, NxReal & planeD, NxReal * restitution = NULL);


	NxJoint *					getNxJoint()	const	{ return mNxJoint; }
	NxJoint *					getOwner()		const	{ return mOwner; }

	bool						commitChanges();
	bool						isAwaitingDelete()						const;
	bool						instance();
	bool						isInstanced()							const;

	void						checkInstanced(const char *fn)			const;
	void						checkWrite(const char *fn)				const;
	void						checkInstancedWrite(const char *fn)		const;
	bool						writable()								const;

	class SceneIndexer
	{
	public:
		static NxU32 &index(NxdJointShared &j)		{	return j.mSceneIndex;		}
	};

	class DirtyIndexer
	{
	public:
		static NxU32 &index(NxdJointShared &j)		{	return j.mDirtyIndex;		}
	};

	void setPointers(NxJoint *joint, NxJointDesc *desc);

	void updateDescFromBufferedData(NxJointDesc &desc);
	void loadFromDescNotify();

private:

	enum
	{
		BUFFERED_ANCHOR							= (1<<0),
		BUFFERED_AXIS							= (1<<1),
		BUFFERED_BREAKABLE						= (1<<2),
		BUFFERED_SOLVER_EXTRAPOLATION_FACTOR	= (1<<3),
		BUFFERED_USE_ACCELERATION_SPRING		= (1<<4)
	};

	NxJoint *		mNxJoint;
	NxJointDesc *	mDesc;

	NxJoint *		mOwner;
	NxdScene &		mNxdScene;
	NxU32			mSceneIndex;
	NxU32			mDirtyIndex;
	NxU32			mBufferFlags;
	NxVec3			mGlobalAnchor;
	NxVec3			mGlobalAxis;
};

NXD_DECLARE_BUFFERED_CASTS(NxJoint)

#endif
