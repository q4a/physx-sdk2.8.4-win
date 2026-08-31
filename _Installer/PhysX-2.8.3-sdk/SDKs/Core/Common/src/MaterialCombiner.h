#ifndef NX_PHYSICS_MATERIALCOMBINER
#define NX_PHYSICS_MATERIALCOMBINER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Physics.h"
#include "NxMaterial.h"
/**
This class provides functionality to combine two surface materials into a contact material
*/
class Shape;
class Scene;

class MaterialCombiner
	{
	public:
	class CombinedRestitution
		{
		public:
		NxReal restitution;
		NxU32  programmable;		//TODO: temp design!?
		};

	class CombinedMaterial
		{
		public:
		NxReal staFrictionT1;
		NxReal staFrictionT2;
		NxReal dynFrictionT1;
		NxReal dynFrictionT2;
		NxVec3 dirOfAnisotropyWS;
		NxU32	flags;				//NX_MF_ANISOTROPIC, NX_MF_DISABLE_FRICTION, NX_MF_DISABLE_STRONG_FRICTION.
		};

	static void combineRestitution(Scene &, NxU32 materialIndexPair, CombinedRestitution & dest);
	static void combineRestitution(const NxMaterialDesc& material0, const NxMaterialDesc& material1, CombinedRestitution & dest);
	static void combineFriction(Scene &, const Shape ** shapes, NxU32 materialIndexPair, const NxVec3 & contactNormal, CombinedMaterial & dest, NxVec3 & t0, NxVec3 & t1); //shape_a has material low word, shape_b has high word.
	static void combineIsotropicFriction(Scene &, const Shape ** shapes, NxU32 materialIndexPair, CombinedMaterial & dest); 
	static void combineIsotropicFriction(const NxMaterialDesc& material0, const NxMaterialDesc& material1, CombinedMaterial & dest); 
	static void combineAnisotropicFriction(const Shape & anisotropicShape, const NxMaterialDesc& materialAniso, const NxMaterialDesc& materialIso, CombinedMaterial & dest);
	static void combineAnisotropicFriction(Scene &, const Shape ** shapes, const NxMaterialDesc& material0, const NxMaterialDesc& material1, const NxVec3 & contactNormal, CombinedMaterial & dest, NxVec3 & t0, NxVec3 & t1); //shape_a has material low word, shape_b has high word.
	private:
	static NxReal combineScalars(NxReal a, NxReal b, NxI32 nxCombineMode);
	};

#endif
