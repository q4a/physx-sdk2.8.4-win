#ifndef NX_PHYSICS_NP_FORCEFIELDSHAPEGROUP
#define NX_PHYSICS_NP_FORCEFIELDSHAPEGROUP
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "ReadWriteMutex.h"
#include "Array.h"
#include "NxBounds3.h"
#include "NxForceFieldShapeGroup.h"
#include "NxForceFieldShapeGroupDesc.h"
#include "NpForceField.h"

class NpScene;
class NpForceFieldShape;
struct ShapeList;
 

class NpForceFieldShapeGroup: public NxForceFieldShapeGroup, public NxFoundation::NxAllocateable, public ReadWriteMutex
	{
	public:
	typedef  	NxFoundation::NxArraySDK<NpForceField*>	ForceFieldArray;

										NpForceFieldShapeGroup(const NxForceFieldShapeGroupDesc& desc, NpScene& npScene, NpForceField* ff);
	virtual								~NpForceFieldShapeGroup();

	//NxForceFieldShapeGroup
	virtual		NxForceFieldShape*		createShape(const NxForceFieldShapeDesc &);
	virtual		void					releaseShape(const NxForceFieldShape &);
	virtual		NxU32					getNbShapes() const; 
	virtual		void					resetShapesIterator(); 
	virtual		NxForceFieldShape*		getNextShape(); 
	virtual		NxForceFieldShape*		getShapeByIndex(NxI32 iIndex);
	virtual		NxU32					getFlags() const;
	virtual		NxForceField*			getForceField() const; 
	virtual		NxScene&				getScene() const;
	virtual		void					setName(const char* name);
	virtual		const char*				getName() const;
	virtual		void					saveToDesc(NxForceFieldShapeGroupDesc &desc);
	//~NxForceFieldShapeGroup
				void					visualize(NxFoundation::DebugRenderable& dr)	const;
				bool					updateBounds();
				void					prepareShapes();
				bool					contains(const NxVec3& pos) const;

	// BroadPhase interaction
				bool					addTouchedBounds(const NvRawBounds& bounds);
				bool					removeTouchedBounds(const NvRawBounds& bounds);
				bool					addTouchedShape(const NvShape& shape);
				bool					removeTouchedShape(const NvShape& shape);
	
	NX_INLINE	bool					isIncludeGroup() const	{ return mIncludeGroup; }
	NX_INLINE	ForceFieldArray&		getForceFieldArray()	{ return mOwners;		}
	NX_INLINE	NxU32					_getFlags() const		{ return mFlags;		}
	NX_INLINE	const NxBounds3&		getBounds()	const		{ return mBounds;		}
	NX_INLINE	bool					isBoundsValid()	const	{ return mValidBounds;	}
	NX_INLINE	void					invalidateBounds()		
																{ 
																if(mValidBounds && !(mFlags & NX_FFSG_EXCLUDE_GROUP))
																	{
																	for(NxU32 i = 0; i < mOwners.size(); i++)
																		mOwners[i]->invalidateBounds();
																	}
																mValidBounds = false;
																}
	NX_INLINE	void					addOwner(NpForceField* ff)		{ mOwners.pushBack(ff); }
	NX_INLINE	bool					removeOwner(NpForceField* ff)	{ return mOwners.deleteEntry(ff); }
	NX_INLINE	void*					getMirrorInfo()			const	{ return mMirrorInfo;			}
	NX_INLINE	void					setMirrorInfo(void*i)			{ mMirrorInfo = i;				}

	NX_INLINE	NxU32					getNbTouchedObjects() const		{ return mTouchedObjects.GetNbEntries(); }
	NX_INLINE	const void**			getTouchedObjects() const		{ return (const void**)mTouchedObjects.GetEntries(); }
	NX_INLINE	NpScene&				getNpScene()			{ return mNpScene; }

	private:
				NxForceFieldShape*		_createShape(const NxForceFieldShapeDesc &);

	NX_INLINE	ShapeList*				createShapeList(const NvShape* nvShape);
	NX_INLINE	void					removeShapeList(ShapeList* s)
	{
		mShapeListPool.pushBack(s);
	}

	private:
	
	NxU32								mFlags;
	NxFoundation::NxArraySDK<const NpForceFieldShape*>
										mShapes;
	NxI32								mShapeIter;
	NxBounds3							mBounds;
	ContainerSizeT						mTouchedObjects;		// Things currently touching the force field
	NxFoundation::NxArraySDK<ShapeList*>						
										mShapeListPool;
	bool								mValidBounds;
	bool								mIncludeGroup;

	NpScene&							mNpScene;
	ForceFieldArray						mOwners;
	void*								mMirrorInfo; //hook for mirroring of this forcefield.  Used by HardwareAbstraction
	};




// list to collect compound shapes in the mTouchedObject array
struct ShapeList: public NxFoundation::NxAllocateable
{
	ShapeList(const NvShape* s)
		: shape(s)
		, next(NULL) {}

	NX_INLINE void appendEnd(ShapeList* s)
	{
		NX_ASSERT(this->next == NULL);
		this->next = s;
	}

	static NX_INLINE void remove(ShapeList* prev, ShapeList* rem)
	{
		NX_ASSERT(prev && rem && prev->next == rem);
		prev->next = rem->next;
	}

	const NvShape*	shape;
	ShapeList*		next;
};

NX_INLINE ShapeList* NpForceFieldShapeGroup::createShapeList(const NvShape* nvShape)
{
	if(mShapeListPool.size())
	{
		ShapeList* ret = mShapeListPool.back();
		mShapeListPool.popBack();
		*ret = ShapeList(nvShape);
		return ret;
	}
	else
	{
		return NX_NEW_MEM(ShapeList, PERSISTENT)(nvShape);
	}
}


#endif
