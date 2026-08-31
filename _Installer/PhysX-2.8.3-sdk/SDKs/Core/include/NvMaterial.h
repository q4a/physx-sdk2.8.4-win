#ifndef NX_PHYSICS_NV_MATERIAL
#define NX_PHYSICS_NV_MATERIAL
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxMaterialDesc.h"

class NxMaterial;

class NvMaterialDesc : public NxMaterialDesc	//its exactly the same thing so why not leverage the other class?
	{
	public:
	NxMaterial *	nxMaterial;					//NULL for default material.
	};

class NvMaterial
	{
	public:

	virtual		NxMaterialIndex getMaterialIndex() = 0;
	virtual		void			loadFromDesc(const NxMaterialDesc&) = 0;
	virtual		void			saveToDesc(NxMaterialDesc&) const	= 0;
	virtual		void			setDynamicFriction(NxReal) = 0;
	virtual		NxReal			getDynamicFriction() const = 0;
	virtual		void			setStaticFriction(NxReal) = 0;
	virtual		NxReal			getStaticFriction() const = 0;
	virtual		void			setRestitution(NxReal) = 0;
	virtual		NxReal			getRestitution() const = 0;
	virtual		void			setDynamicFrictionV(NxReal) = 0;
	virtual		NxReal			getDynamicFrictionV() const = 0;
	virtual		void			setStaticFrictionV(NxReal) = 0;
	virtual		NxReal			getStaticFrictionV() const = 0;
	virtual		void			setDirOfAnisotropy(const NxVec3 &) = 0;
	virtual		NxVec3			getDirOfAnisotropy() const = 0;
	virtual		void			setFlags(NxU32) = 0;
	virtual		NxU32			getFlags() const = 0;
	virtual		void			setFrictionCombineMode(NxCombineMode) = 0;
	virtual		NxCombineMode	getFrictionCombineMode() const = 0;
	virtual		void			setRestitutionCombineMode(NxCombineMode) = 0;
	virtual		NxCombineMode	getRestitutionCombineMode() const = 0;
	//virtual		void			setSpring(const NxSpringDesc &) = 0;
	//virtual		NxSpringDesc	getSpring() const = 0;


	virtual		void			setNxMaterial(NxMaterial*) = 0;
	virtual		NxMaterial*		getNxMaterial() = 0;
	};



#endif
