#ifndef NX_PHYSICS_NP_MATERIAL
#define NX_PHYSICS_NP_MATERIAL
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


#include "NxMaterial.h"
#include "Allocateable.h"
#include "ReadWriteMutex.h"

class NvMaterial;
class NpScene;

class NpMaterial : public NxMaterial, public NxFoundation::NxAllocateable, public ReadWriteMutex
	{
	public:
	NpMaterial(NvMaterial * m, NxMaterialIndex i, NpScene	* );
	~NpMaterial();

	virtual		NxMaterialIndex getMaterialIndex();
	virtual		void			loadFromDesc(const NxMaterialDesc&);
	virtual		void			saveToDesc(NxMaterialDesc&) const;
	virtual		NxScene&		getScene() const;
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


	NX_INLINE	NvMaterial *	getNvMaterial()	{ return material; }
	NX_INLINE	NxMaterialIndex	getIndexFast()	{ return index; }

				void			sendToVRD(bool create);
	private:
	void postChange();

	NvMaterial * material;
	NxMaterialIndex index;
	NpScene	* scene;		//just to implement getScene().
	};

#endif
