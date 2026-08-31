#ifndef NX_PHYSICS_FLUIDBODYINTERACTION
#define NX_PHYSICS_FLUIDBODYINTERACTION
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxp.h"
#if NX_USE_FLUID_API

#include "ElementInteraction.h"
#include "FluidPacketShape.h"
#include "Shape.h"


class FluidElementRbElementInteraction : public ElementInteraction
{
public:

	FluidElementRbElementInteraction(FluidPacketShape &fluidShape, Shape& rbShape);
	virtual ~FluidElementRbElementInteraction();
	NX_INLINE void* operator new(size_t s, void* memory);

	virtual void initialize();
	virtual void destroy();

	NX_INLINE FluidPacketShape& getFluidShape() const;
	NX_INLINE Shape& getRbShape() const;

	virtual bool isActive() const { return false; }	// MS: Mark as inactive since fluid vs. body interactions should not affect
													//     things like island generation

#ifdef NX_FLUID_IN_PRIMARY_SCENE
	static bool collisionFiltering(const FluidPacketShape& fluidShape, const Shape& shape);
#endif

	void update();

protected:
	void onActivate() {}
	void onDeactivate() {}

private:
	bool isCollisionValid(const FluidPacketShape& fluidShape, const Shape& shape);
	void createContactManager(const Shape& shape);
	void removeContactManager();


		PxdManager			mLLContactManager;		// Low level contact manager handle.
};


NX_INLINE void* FluidElementRbElementInteraction::operator new(size_t s, void* memory)
{
	return memory;
}

NX_INLINE FluidPacketShape& FluidElementRbElementInteraction::getFluidShape() const
{
	return static_cast<FluidPacketShape&>(getElement0());
}

NX_INLINE Shape& FluidElementRbElementInteraction::getRbShape() const
{
	return static_cast<Shape&>(getElement1());
}


#endif	// NX_USE_FLUID_API

#endif

//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND
