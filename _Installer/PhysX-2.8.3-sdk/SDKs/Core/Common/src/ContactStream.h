#ifndef NX_COLLISION_CONTACTSTREAM
#define NX_COLLISION_CONTACTSTREAM
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxContactStreamIterator.h"
#include "Shape.h"
#include "FPU.h"

//#define DO_CENTROID_TWIST_FRICTION

#ifdef DO_CENTROID_TWIST_FRICTION
#error contact stream not implemented for this case
#endif

// seems slightly faster
NX_INLINE bool different(const NxVec3& v0, const NxVec3& v1)
	{
	return ((NX_IR(v0.x) != NX_IR(v1.x))||(NX_IR(v0.y) != NX_IR(v1.y))||(NX_IR(v0.z) != NX_IR(v1.z)));
	}

class ContactStream
	{
	public:
						ContactStream();
						~ContactStream();

				void	reset();

	// Use those from contact generation routines to bypass checkings (provided you know what you're doing)
//	private:
	NX_INLINE	void	submitShapes(const Shape* s0, const Shape* s1, NxU32 shapeFlags=0)
						{
						NX_ASSERT(!(shapeFlags&0xffffff00));

						//AM: new: support NX_SF_POINT_CONTACT_FORCE flag:
						shapeFlags |= s0->getFlagFast(NX_SF_POINT_CONTACT_FORCE);
						shapeFlags |= s1->getFlagFast(NX_SF_POINT_CONTACT_FORCE);

						bool deletedShape = (s0->isDying() || s1->isDying());
						if (deletedShape)
						{
							containsDeletedShapes = true;
							shapeFlags |= NX_SF_DELETED_SHAPE_0;  // temporary marker to know that this shape pair needs further processing
						}

						currentShapesFlags = shapeFlags;

						shapeFlags<<=16;

						// Here the shapes & normal have already been fixed
//						if(currentShape0!=s0 || currentShape1!=s1)	// Also covers first run when shape0==shape1==null
							{
							// Close previous pair
							// Open new pair
							currentShape0 = s0;
							currentShape1 = s1;
							if (!deletedShape)
							{
								output(reinterpret_cast<const void*>(s0->getNxShapeConst()));//user will only be able to read NxShape ptrs.
								output(reinterpret_cast<const void*>(s1->getNxShapeConst()));
							}
							else
							{
								// We keep internal shapes around after deletion but not the external ones. Thus we can check later which
								// shapes have been deleted
								output(reinterpret_cast<const void*>(s0));
								output(reinterpret_cast<const void*>(s1));
							}
							//We submit a format flag word here, which is the high 16 bits of the numPatches dword value.
							currentNbPatchesIndex = reserveSlot(shapeFlags);
							increaseSlot(currentNbPairsIndex);

							// Reset patch
							// PT: zero normals are not enough since some bugged contact routines sometimes output zero normals too.
							// This creates really nasty bugs where the number of pairs is increased instead of the number of points (!).
							// Using a clearly invalid normal fixes it. See TTP 62 in PhysX_BETA_2.3.1.
//							currentPatchNormal.zero();
							((NxU32&)currentPatchNormal.x) = 0xffffffff;	// NAN
							((NxU32&)currentPatchNormal.y) = 0xffffffff;	// NAN
							((NxU32&)currentPatchNormal.z) = 0xffffffff;	// NAN
							}
						}

	NX_INLINE	void	submitNormal(const NxVec3& contactNormal)
						{
					//	if(currentPatchNormal != contactNormal)	// Also covers the first run when currentPatchNormal == zero
//						if(different(currentPatchNormal, contactNormal))	// Also covers the first run when currentPatchNormal == zero
							{
							// Close previous patch

							// Open new patch
							currentPatchNormal = contactNormal;
							output(contactNormal);

							currentNbContactPointsIndex = reserveSlot(0);

							increaseSlotLow16(currentNbPatchesIndex);

							// Reset point
							}
						}
	NX_INLINE	void	submitPoint(NxReal separation, const NxVec3& contactPoint, NxReal force = 0.0f)
						{
						NX_ASSERT(separation<=0.0f);
						numContacts ++;
						// Close previous contact point
						// Open new contact point
//						NX_IR(contactPoint.x)&=~1;
//						NX_IR(contactPoint.y)&=~1;
//						NX_IR(contactPoint.z)&=~1;
						output(contactPoint);
						NxU32 binary = NX_IR(separation);
						binary &= ~NX_SIGN_BITMASK;
						output(binary);
						increaseSlot(currentNbContactPointsIndex);

						//emit material IDs per point only if needed.
//						if(currentShapesFlags & NX_SF_HAS_MATS_PER_POINT)	output(extData);
						if(currentShapesFlags & NX_SF_POINT_CONTACT_FORCE)  output(force);
						}

	NX_INLINE	void	submitPoint(NxReal separation, const NxVec3& contactPoint, NxU32 featureIndex0, NxU32 featureIndex1, NxReal force = 0.0f)
						{
						NxU32 is32bits;
						if(featureIndex0>0xffff || featureIndex1>0xffff)	is32bits = NX_SIGN_BITMASK;
						else												is32bits = 0;

						NX_ASSERT(separation<=0.0f);
						numContacts ++;
						// Close previous contact point
						// Open new contact point
						output(contactPoint);

						NxU32 binary = NX_IR(separation);	//AM: Pierre, this is really evil. Can't you just use the per shape pair flags field for this?
						binary &= ~NX_SIGN_BITMASK;
						binary |= is32bits;
						output(binary);

						increaseSlot(currentNbContactPointsIndex);

						//emit material IDs per point only if needed.
//						if(currentShapesFlags & NX_SF_HAS_MATS_PER_POINT)	output(extData);
						if(currentShapesFlags & NX_SF_POINT_CONTACT_FORCE)  output(force);
						if(currentShapesFlags & NX_SF_HAS_FEATURES_PER_POINT)
							{
							if(is32bits)
								{
								output(featureIndex0);
								output(featureIndex1);
								}
							else
								{
								output((featureIndex1<<16) | featureIndex0);
								}
							}
						}


	//same thing with custom material params
	NX_INLINE	void	submitContact(const Shape* s0, const Shape* s1, NxReal separation, const NxVec3& contactPoint, const NxVec3& contactNormal, NxReal force = 0.0f)
						{
						// Here the shapes & normal have already been fixed
						if(currentShape0!=s0 || currentShape1!=s1)	// Also covers first run when shape0==shape1==null
							{
							NxU32 matPerPoint = 0;
							submitShapes(s0, s1, matPerPoint);
							}

						if(different(currentPatchNormal, contactNormal))	// Also covers the first run when currentPatchNormal == zero
							submitNormal(contactNormal);

						submitPoint(separation, contactPoint, force );	//note how mat 2 goes into the high word.
						}

	NX_INLINE	void	submitContact(const Shape* s0, const Shape* s1, NxReal separation, const NxVec3& contactPoint, const NxVec3& contactNormal, NxU32 featureIndex0, NxU32 featureIndex1, NxReal force = 0.0f)
						{
						// Here the shapes & normal have already been fixed
						if(currentShape0!=s0 || currentShape1!=s1)	// Also covers first run when shape0==shape1==null
							{
							NxU32 matPerPoint = 0;
							NxU32 featPerPoint = (s0->getFlagFast(NX_SF_FEATURE_INDICES) || s1->getFlagFast(NX_SF_FEATURE_INDICES)) ? NX_SF_HAS_FEATURES_PER_POINT : 0;
							submitShapes(s0, s1, matPerPoint|featPerPoint);
							}

						if(different(currentPatchNormal, contactNormal))	// Also covers the first run when currentPatchNormal == zero
							submitNormal(contactNormal);

						submitPoint(separation, contactPoint, featureIndex0, featureIndex1, force);
						}
	NX_INLINE	void	pokeStream(NxU32 offset, NxReal value)
						{
						data[offset] = NX_IR(value);
						}

				NxU32			numContacts;
				NxU32			currentNbPairsIndex;
				NxU32			currentNbPatchesIndex;
				NxU32			currentNbContactPointsIndex;
	const		Shape*			currentShape0;
	const		Shape*			currentShape1;
				NxVec3			currentPatchNormal;
				NxU32			currentShapesFlags;
#ifdef DO_CENTROID_TWIST_FRICTION
				NxVec3			currentPatchCentroid;	// GLOBALPOS
#endif

	NX_INLINE	const NxU32*	getBuffer()		const	{ return data.GetEntries();						}
	NX_INLINE	NxU32			getBufferSize()	const	{ return data.GetNbEntries() * sizeof(udword);	}

	NX_INLINE	bool			readContainsDeletedShape() { return containsDeletedShapes; }

	private:
				Container		data;	// GLOBALPOS
				bool			containsDeletedShapes;

#ifdef NX32
	NX_INLINE	void	output(const void* ptr)	{ data.Add(udword(size_t(ptr))); }
#else
	NX_INLINE	void	output(const void* ptr)
	{
		NxU64 bits = NxU64(ptr);
		data.Add(udword(bits)).Add(udword(bits>>32));
	}
#endif
	NX_INLINE	void	output(NxU32 d)					{ data.Add(d);				}
	NX_INLINE	void	output(NxReal f)				{ data.Add(f);				}
	NX_INLINE	void	output(const NxVec3& point)		{ data.Add(&point.x, 3);	}
	NX_INLINE	void	increaseSlot(NxU32 index)		{ data[index]++;			}
	NX_INLINE	void	increaseSlotLow16(NxU32 index)	{ data[index]++;			}//Here we use the high 16 bits for something else, so we only want to change the low 16. We actually don't need extra code because this will only overflow the low 16 bits if we have more than 64K items, which is unlikely.
	NX_INLINE	NxU32	reserveSlot(NxU32 value)
						{
						NxU32 index = data.GetNbEntries();
						data.Add(value);
						return index;
						}
	};

#endif
