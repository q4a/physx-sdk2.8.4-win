#ifndef PXS_SHAPE_TRANSFORM_HASH_H
#define PXS_SHAPE_TRANSFORM_HASH_H


#include "PxcTransform.h"
#include "PxsBodyShape.h"

#define PXS_SHAPE_TRANSFORM_HASH_SIZE		1024	// Size of hash table for last frame's world to shape transforms
													// NOTE: Needs to be power of 2

/*!
Structure to store the current and the last frame's world to shape transformations
for shapes that collide with fluids. This information is required in the fluid collision code.
*/
class PxsShapeTransformHash
{
public:
												PxsShapeTransformHash();
												~PxsShapeTransformHash();

						void					addShape(const PxsBodyShape& bodyShape);
						void					removeShape(const PxsBodyShape& bodyShape);
						void					getTransform(const PxsBodyShape& bodyShape, PxcTransform& w2s) const;
						void					getOldTransform(const PxsBodyShape& bodyShape, PxcTransform& w2s) const;
						void					update();

private:

	struct PxsWorld2Shape
	{
		PxcTransform		w2s;		// Current frame's world to shape transformation
		PxcTransform		w2sOld;		// Last frame's world to shape transformation
		PxsWorld2Shape*		next;		// Next hash entry (used if some entries share the same hash index)
		const PxsBodyShape*	shape;		// The rigid body shape
	};

	PXD_FORCE_INLINE	PxU32					getHashIndex(const PxsBodyShape& bodyShape) const;
	PXD_FORCE_INLINE	void					computeTransform(const PxsBodyShape& bodyShape, PxcTransform& w2s);
	PXD_FORCE_INLINE	PxsWorld2Shape*			createEntry(const PxsBodyShape& bodyShape);
	PXD_FORCE_INLINE	bool					findTransform(const PxsBodyShape& bodyShape, PxsWorld2Shape*& w2sEntry,
																PxsWorld2Shape*& prevEntry) const;

	PxsWorld2Shape*								mWorld2ShapeHash[PXS_SHAPE_TRANSFORM_HASH_SIZE];	// Hash table for last frames world to shape transforms.
	PxcSimplePool<PxsShapeTransformHash::PxsWorld2Shape>	mWorld2ShapePool;	// Pool of last frames world to shape transforms. Used for
																				// shapes that collide with fluids.
};


PXD_FORCE_INLINE PxU32 PxsShapeTransformHash::getHashIndex(const PxsBodyShape& bodyShape) const
{
	PxU32 index = bodyShape.getIndex();
	return (index & (PXS_SHAPE_TRANSFORM_HASH_SIZE - 1));	// Modulo hash size
}

PXD_FORCE_INLINE void PxsShapeTransformHash::computeTransform(const PxsBodyShape& bodyShape, PxcTransform& w2s)
{
	const PxsBodyAtom* bodyAtom = bodyShape.getBodyAtom();

	if (!bodyAtom)
		w2s = bodyShape.getTransform().getInverse();
	else
		w2s = bodyShape.getTransform().transformInv( bodyAtom->getPose().getInverse() );
}

PXD_FORCE_INLINE PxsShapeTransformHash::PxsWorld2Shape* PxsShapeTransformHash::createEntry(const PxsBodyShape& bodyShape)
{
	PxsWorld2Shape* w2sEntry = mWorld2ShapePool.construct();

	if (w2sEntry)
	{
		computeTransform(bodyShape, w2sEntry->w2s);
		w2sEntry->next = NULL;
		w2sEntry->shape = &bodyShape;
	}

	return w2sEntry;
}

PXD_FORCE_INLINE bool PxsShapeTransformHash::findTransform(const PxsBodyShape& bodyShape, PxsWorld2Shape*& w2sEntry,
														PxsWorld2Shape*& prevEntry) const
{
	PxU32 hashIndex = getHashIndex(bodyShape);

	prevEntry = NULL;
	bool hasEntry = false;
	if (mWorld2ShapeHash[hashIndex])
	{
		PxsWorld2Shape* curEntry = mWorld2ShapeHash[hashIndex];

		while(curEntry->next)
		{
			if (curEntry->shape == &bodyShape)
				break;

			prevEntry = curEntry;
			curEntry = curEntry->next;
		}

		w2sEntry = curEntry;
		if (curEntry->shape == &bodyShape)
		{
			// An entry already exists for the given shape
			hasEntry = true;
		}
	}
	else
	{
		w2sEntry = NULL;
	}

	return hasEntry;
}


#endif
