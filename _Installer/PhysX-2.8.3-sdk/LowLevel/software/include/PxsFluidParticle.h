#ifndef PXS_FLUID_PARTICLE_H
#define PXS_FLUID_PARTICLE_H


#include "px_types.h"
#include "PxcArray.h"
#include "PxcVector.h"
#include "PxcAlignedMalloc.h"
#include "PxcGridCellVector.h"
#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS

#include "PxsFluidCollisionData.h"


// NOTE: Vector fields of this structure should be 16byte aligned,
// this implies that the size of this structure should be a multiple of 16bytes.
struct PxsFluidParticle
{
	PxcVector			position;
	PxdF32				density;
	//16

	PxcVector			velocity;
	PxdF32				deathTime;
	//32

	PxcVector			forceOrNormal;
	PxU16				id;
	PxU16				flags;
	//48

	PxcGridCellVector	cell;
	PxU16				hashKey;			// Will be used to temporarily store indices for a particle

	// IMPORTANT: Do NOT use items in unions that have non default constructors (because not officially supported in C++)!
	union
	{
		PxdF32			pressure;			// Set during dynamics update. Temporary storage of pressure for speedup.
		PxdShape		shapeId;			// Set during collision update. Handle of colliding shape.
	};
	//PxdF32				invDensity;
	PxU16				constraintIdx;
	PxU8				constraint0Info;	// Additional information on first constraint
	PxU8				constraint1Info;	// Additional information on second constraint
	//64


public:
	PXD_FORCE_INLINE PxsFluidParticle& operator= (const PxsFluidParticle& r)
	{
		memcpy(this, &r, sizeof(PxsFluidParticle));
		return *this;
	}

	PXD_FORCE_INLINE void init(const PxdF32* _position, const PxdF32* _velocity, PxdF32 _deathTime, PxU16 _id)
	{
		position.set(_position[0], _position[1], _position[2]);
		velocity.set(_velocity[0], _velocity[1], _velocity[2]);

		deathTime = _deathTime;
		id = _id;
		flags = 0;
		constraintIdx = PXS_FLUID_NO_CONSTRAINT;
		constraint0Info = PXS_FLUID_CONSTRAINT_INVALID;
		constraint1Info = PXS_FLUID_CONSTRAINT_INVALID;
	}
};

PXD_COMPILE_TIME_ASSERT(sizeof(PxdF32) == sizeof(PxdShape))


class PxsFluidParticleArray
{
public:

	PxsFluidParticleArray(PxU16 particlesMax):
		mParticlesMax(particlesMax),
		mParticlesNum(0)
	{ 
		mParticles = reinterpret_cast<PxsFluidParticle*>(PxcAlignedMalloc(mParticlesMax * sizeof(PxsFluidParticle), PARTICLE_ALIGNMENT));
	}

	PxsFluidParticleArray():
		mParticlesMax(0),
		mParticlesNum(0),
		mParticles(NULL)
	{
	}
	
	~PxsFluidParticleArray()
	{
		free();
	}

	void free()
	{
		if (mParticles)
		{
			PxcAlignedFree(mParticles);
			mParticles = NULL;
		}

		mParticlesMax = 0;
		mParticlesNum = 0;
	}

	void setCapacity(PxU16 particlesMax)
	{
		//its not possible to resize an array, which has elements in it by now.
		PXN_ASSERT(mParticlesNum == 0);

		if (mParticles)
		{
			PxcAlignedFree(mParticles);
		}

		mParticlesMax = particlesMax;
		mParticles = reinterpret_cast<PxsFluidParticle*>(PxcAlignedMalloc(mParticlesMax * sizeof(PxsFluidParticle), PARTICLE_ALIGNMENT));
	}

	void setSize(PxU16 numParticles)
	{
        PXN_ASSERT(mParticlesMax >= numParticles);
		mParticlesNum = numParticles;
	}

	PxU16 getSize() const { return mParticlesNum; }
	
	const PxsFluidParticle* getParticles() const { return mParticles; }
	PxsFluidParticle* getParticles() { return mParticles; }
	
	const PxsFluidParticle& operator[](PxU16 index) const { PXN_ASSERT(index < mParticlesMax); return mParticles[index]; }
	PxsFluidParticle& operator[](PxU16 index) { PXN_ASSERT(index < mParticlesMax); return mParticles[index]; }

	PxsFluidParticle& pushBack(PxsFluidParticle& particle) { PXN_ASSERT(mParticlesNum < mParticlesMax); return mParticles[mParticlesNum++] = particle; }
	PxsFluidParticle& pushBack() { PXN_ASSERT(mParticlesNum < mParticlesMax); return mParticles[mParticlesNum++]; }

	void replaceWithLast(PxU16 index)
	{
		PXN_ASSERT(index < mParticlesNum);
		mParticles[index] = mParticles[mParticlesNum-1];
		mParticlesNum--;
	}

private:

	static const PxU32 PARTICLE_ALIGNMENT = PXS_FLUID_DATA_ALIGNMENT;

	PxU16 mParticlesMax;
	PxU16 mParticlesNum;
	
	PxsFluidParticle* mParticles;

};


#endif	// PXS_ENABLE_FLUIDS

#endif
