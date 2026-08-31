#ifndef NX_PHYSICS_MATERIAL
#define NX_PHYSICS_MATERIAL
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxMaterialDesc.h"
#include "NvMaterial.h"
#include "Allocateable.h"


class Material : public NvMaterial, public NxMaterialDesc, public NxFoundation::NxAllocateable
	{
	public:
	Material(NxMaterialIndex);
	~Material();

	virtual		NxMaterialIndex getMaterialIndex();
	virtual		void			loadFromDesc(const NxMaterialDesc&);
	virtual		void			saveToDesc(NxMaterialDesc&) const;
	virtual		void			setDynamicFriction(NxReal);
	virtual		NxReal			getDynamicFriction() const;
	virtual		void			setStaticFriction(NxReal);
	virtual		NxReal			getStaticFriction() const;
	virtual		void			setRestitution(NxReal);
	virtual		NxReal			getRestitution() const;
	virtual		void			setDynamicFrictionV(NxReal);
	virtual		NxReal			getDynamicFrictionV() const;
	virtual		void			setStaticFrictionV(NxReal);
	virtual		NxReal			getStaticFrictionV() const;
	virtual		void			setDirOfAnisotropy(const NxVec3 &);
	virtual		NxVec3			getDirOfAnisotropy() const;
	virtual		void			setFlags(NxU32);
	virtual		NxU32			getFlags() const;
	virtual		void			setFrictionCombineMode(NxCombineMode);
	virtual		NxCombineMode	getFrictionCombineMode() const;
	virtual		void			setRestitutionCombineMode(NxCombineMode);
	virtual		NxCombineMode	getRestitutionCombineMode() const;
	//virtual		void			setSpring(const NxSpringDesc &);
	//virtual		NxSpringDesc	getSpring() const;

	virtual		void			setNxMaterial(NxMaterial*);
	virtual		NxMaterial*		getNxMaterial();

	NxMaterialIndex				index;
	NxMaterialIndex				nextFreeSlot;		//index of next known available slot in the material array.
	NxMaterial *				userPtr;
	};

#endif
