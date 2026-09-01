#ifndef __NX_EMITTER_GEOM_H__
#define __NX_EMITTER_GEOM_H__
/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#include "NxApex.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

///Emitter type
struct NxApexEmitterType
{
	enum Enum
	{
		NX_ET_RATE = 0,			///< use emittion rate to calculate the number of particles
		NX_ET_DENSITY_ONCE,		///< emit density particles at once
		NX_ET_DENSITY_BRUSH,	///< brush behavior: particles are emitted proportionally to the newly occupied vlume
		NX_ET_FILL,				///< fill the volume of the emitter with particles
		NX_ET_UNDEFINED,
	};
};

class NxEmitterBoxGeom;
class NxEmitterSphereGeom;
class NxEmitterSphereShellGeom;
class NxEmitterCylinderGeom;
class NxEmitterExplicitGeom;

///Base class for all emitter shapes
class NxEmitterGeom
{
public:
	///Sets the type of the emitter
    virtual void								setEmitterType( NxApexEmitterType::Enum ) = 0;
    ///Gets the type of the emitter
    virtual NxApexEmitterType::Enum				getEmitterType() const = 0;

	///If it is a box, cast to box class, return NULL otherwise
	virtual const NxEmitterBoxGeom *			isBoxGeom() const			{ return NULL; }
	///If it is a sphere, cast to sphere class, return NULL otherwise
	virtual const NxEmitterSphereGeom *			isSphereGeom() const		{ return NULL; }
	///If it is a sphere shell, cast to sphere shell class, return NULL otherwise
	virtual const NxEmitterSphereShellGeom *	isSphereShellGeom() const	{ return NULL; }
	///If it is a cylinder shell, cast to cylinder class, return NULL otherwise
	virtual const NxEmitterCylinderGeom *		isCylinderGeom() const		{ return NULL; }
	///If it is an explicit geom, cast to explicit geom class, return NULL otherwise
	virtual const NxEmitterExplicitGeom *		isExplicitGeom() const		{ return NULL; }
};

///Sphere shell shape for an emitter. It's a shape formed by the difference of two cocentered spheres.
class NxEmitterSphereShellGeom : public NxEmitterGeom
{
public:
    virtual void setRadius(physx::PxF32 radius) = 0;
    virtual physx::PxF32 getRadius() const = 0;

    virtual void setShellThickness(physx::PxF32 thickness) = 0;
    virtual physx::PxF32 getShellThickness() const = 0;
};

///Spherical shape for an emitter.
class NxEmitterSphereGeom : public NxEmitterGeom
{
public:
	virtual void	setRadius( physx::PxF32 radius ) = 0;
	virtual physx::PxF32	getRadius() const = 0;
};

///Cylindrical shape for an emitter.
class NxEmitterCylinderGeom : public NxEmitterGeom
{
public:
	virtual void	setRadius( physx::PxF32 radius ) = 0;
	virtual physx::PxF32	getRadius() const = 0;
	
	virtual void	setHeight( physx::PxF32 height ) = 0;
	virtual physx::PxF32	getHeight() const = 0;
};

///Box shape for an emitter.
class NxEmitterBoxGeom : public NxEmitterGeom
{
public:
    virtual void	setExtents(const physx::PxVec3& extents) = 0;
    virtual physx::PxVec3	getExtents() const = 0;
};
///Explicit geometry. Coordinates of each particle are given explicitly.
class NxEmitterExplicitGeom : public NxEmitterGeom
{
public:
	///Remove all particles
	virtual void    resetParticleList() = 0;

	/**
	\brief Add particles to geometry to be emitted
	\param [in] count - number of particles being added by this call
	\param [in] relPositions must be specified.  when emitted, these relative positions are added to emitter actor position
	\param [in] velocities if NULL, the geometry's velocity list will be padded with zero velocities and the asset's velocityRange will be used for velocity
	*/
	virtual void    addParticleList( physx::PxU32 count, const physx::PxVec3* relPositions, const physx::PxVec3* velocities = 0 ) = 0;

	///Access the list of particles
	virtual void	getParticleList( const physx::PxVec3* &relPositions, 
									 physx::PxU32 &numPositions, 
									 const physx::PxVec3* &velocities, 
									 physx::PxU32 &numVelocities ) = 0;

	///Get the number of particles
	virtual physx::PxU32   getParticleCount() const = 0;
	///Get the position of the particle
	virtual physx::PxVec3  getParticlePos( physx::PxU32 index ) const = 0;
};


PX_POP_PACK

}} // end namespace physx::apex

#endif
