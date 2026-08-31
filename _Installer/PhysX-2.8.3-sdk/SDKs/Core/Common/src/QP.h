#ifndef NX_PHYSICS_QP
#define NX_PHYSICS_QP
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

// Moved from Body.h
class NxQP
{
public:
	// Important: Ordering of members is important. See Body::LLState
	NxVec3 p;
	NxQuat q;

	NX_INLINE NxQP(): p(0,0,0), q(NxVec3(0,0,0),1)
	{
	}

	NX_INLINE NxQP(const NxQuat &q0, const NxVec3 &p0): q(q0), p(p0)
	{
	}

	NX_INLINE NxQP operator*(const NxQP &x) const
	{
		return NxQP(q*x.q, q.rot(x.p)+p);
	}

	NX_INLINE NxQP operator !() const
	{
		return NxQP(!q, q.invRot(-p));
	}

	NX_INLINE NxVec3 transform(const NxVec3 &input) const
	{
		return q.transform(input, p);
	}

	NX_INLINE NxVec3 invTransform(const NxVec3 &input) const
	{
		return q.invTransform(input, p);
	}
};


#endif
