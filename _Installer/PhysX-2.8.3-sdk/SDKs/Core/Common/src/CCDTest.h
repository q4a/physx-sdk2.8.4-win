#ifndef NX_CCD_TEST
#define NX_CCD_TEST

#include "Physics.h"
#include "CCDSkeleton.h"
#include "Allocateable.h"

class Shape;
class Scene;
class NxBox;
class Body;
class NxQP;
class NPhaseContext;

class CCDTest : public NxFoundation::NxAllocateable
	{
	public:
	
	// Methods to compute the time of impact for the whole body stored in the buffer.
	void	ccdStaticDynamicMesh			(const Shape* dynamicShape, const Shape* staticShape);	//does linear and angular ccd in sequence determined by current scene tick, from body2worldOld to body2World
	NxReal	ccdStaticDynamicAngular			(const Shape* dynamicShape, const Shape* staticShape, NxReal toiSoFar, bool useNewTranslation);	//angular ccd, from body2worldOld.M to body2World.M, using old or new translation (last arg).  ToiSoFar is time < 1 up to which we check for colls.
	NxReal	ccdStaticDynamicLinearFAST		(const Shape* dynamicShape, const Shape* staticShape, NxReal toiSoFar, bool useNewRotation);	//same, but linear case.

	void	ccdDynamicDynamicMesh			(const Shape* dynamicShape0, const Shape* dynamicShape1);	//does linear and angular ccd in sequence determined by current scene tick, from body2worldOld to body2World
	NxReal	ccdDynamicDynamicAngular		(const Shape* dynamicShape0, const Shape* dynamicShape1, NxReal toiSoFar, bool useNewTranslation);
	NxReal	ccdDynamicDynamicLinear			(const Shape* dynamicShape0, const Shape* dynamicShape1, NxReal toiSoFar, bool useNewRotation);

	private:
	static	NxReal	toiSoFar				(Body * b, NX_BOOL haveRotTransSequence, NX_BOOL angular);		//extract our stupidly encoded toi from the body, always returns [0,1]
	static	void	getShapeBounds			(const NxQP& body2World, const NxQP& body2Actor,const Shape * dynamicShape, NxBounds3 & dest);//TODO: may want to put virtual method into shape so compound does this automatically.
	static	void	getShapeOBB				(const NxQP& body2World, const NxQP& body2Actor,const Shape * dynamicShape, NxBox & dest);	//similar to above.
	static	NxU32	getTouchedTrigs			(NPhaseContext* context, Body * dynBody, const Shape* smShape, const NxBox & temporalBox, const NxU32** triangleIndices);	//opcode query
			void	transformDynamicLinear	(const NxQP& body2dest, const NxQP& body2Actor, const Shape * dynamicShape, const NxVec3 & destSpaceMotionDir, const Shape * filterShape = NULL);	//preprocess dynamic shape into scratch buffer for linear test.
			NxReal	computeLinearTOIFASTDS	(Scene & scene, const Shape* staticShape, const NxBox & temporalBox, NxU32 numTrigs, const NxU32* triangleIndices, const NxVec3 & worldLinDir, const NxVec3 & worldLinMotion, NxVec3 & worldContactPoint, NxVec3 & worldContactNormal);	//computes the TOI between the passed trigs (part of staticShape) and the preprocessed dynamicMeshLinear.  If there is a TOI, also computes cp and normal. convention: normal should point toward dynamic object, so that positive push of dynamic separates.
			void	computeAngularTOIDS		(Scene & scene, NxReal & angTOI, NxVec3 & worldContactPoint, NxVec3 & worldContactNormal);	//same as linear case above
			
			NxReal	computeLinearTOIDD		(Scene & scene, const NxVec3 & linDir, const NxVec3 & linMotionB0, NxVec3 & contactPoint, NxVec3 & contactNormal);
			void	computeAngularTOIDD		(Scene & scene, NxReal & angTOI, NxVec3 & contactPoint, NxVec3 & contactNormal);	//same as linear case above

	static	void	stuckageTest			(Body * dynBody, NxReal toi, const NxVec3 & contactPoint, const NxVec3 & contactNormal);	//checks if we have a ccd stuckage, and tries to do something about it.
	static	void	applyCCDImpulse			(Body * dynBody, const NxVec3 & contactPoint, const NxVec3 & contactNormal);				//stuckage unsticker.  Not very nice, try to get rid of it.
	static	void	boundsofRotSweptOBB		(const NxBox & startingBox, const NxReal angle, const NxVec3 & angularDir, const NxVec3 & pivot, const NxMat33 & rotm, NxBounds3 & temporalBounds);




	//scratch buffers used during TOI computation:
	//TODO: currently these have each their memory buffer but we could have them share....

	CCDSkeleton2 dynamicMeshLinear;		//used by linear case only

	//used by angular case, dynamicMesh used for dynamic mesh nr0 ("pseudo-static") for dynamic-dyanmic
	CCDSkeleton dynamicMesh;	
	CCDSkeleton staticMesh;

	//TODO: get rid of these when optimizing the angular case:
	struct DereferencedTriangle
		{
		NX_INLINE DereferencedTriangle(const CCDSkeleton::TriangleInfo & p, const PlueckerLine & l0, const PlueckerLine & l1, const PlueckerLine & l2) : info(p), undirectedLine0(l0), undirectedLine1(l1), undirectedLine2(l2) {}
		//const NxPlane & plane;
		const CCDSkeleton::TriangleInfo & info;
		const PlueckerLine & undirectedLine0;	//CAUTION: these may be flipped edges.  See info's indices' sign bits to see if you need to flip them.
		const PlueckerLine & undirectedLine1;
		const PlueckerLine & undirectedLine2;
		};
	struct DereferencedEdge
		{
		NX_INLINE DereferencedEdge(const CCDSkeleton::EdgeInfo & i, const NxVec3 & v0, const NxVec3 & v1) : info(i), vertex0(v0), vertex1(v1) {}
		const CCDSkeleton::EdgeInfo & info;
		const NxVec3 & vertex0;
		const NxVec3 & vertex1;
		};

	NxVec3 angMotion, pivot, angDir;
	NxReal angMotionMagnitude;
	
	//Temporary data, here to save time

	void	buildStaticSkeletonAngular	(Scene & scene, const Shape* staticShape, const NxBounds3 & temporalBounds, NxU32 numTrigs, const NxU32* triangleIndices);
	void	transformDynamicSkeleton	(const NxQP& body2World, const NxQP& body2Actor, const Shape * dynamicShape);

	NxReal	toiVertexTrigAngularFAST	(const DereferencedTriangle & triangle, const NxVec3 & point, NxVec3 & ip, const NxMat34 & rotSpace, const NxMat34 & invRotSpace) const;
	NxReal	toiEdgeAngularFAST(const DereferencedEdge & staticEdge, const NxVec3 & edge0, const NxVec3 & edge1, NxVec3 & normal, NxVec3 & ip, const NxMat34 & rotSpace, const NxMat34 & invRotSpace);

/*
		void computeLinearTOI(NxReal & linTOI);
		NxReal computeLinearTOIFAST(Scene & scene, const Shape* smShape, const NxBox & temporalBox, NxU32 numTrigs, const NxU32* triangleIndices);
		void computeAngularTOI(NxReal & angTOI);

		enum CCDContactMode
			{
			STATIC_VERTEX,
			DYNAMIC_VERTEX,
			//EDGE_EDGE,
			//PENETRATION
			};

		// Methods that are called for one pair of point/triangle or edge/edge.
		NxReal toiVertexTrigLinear (const DereferencedTriangle &, const NxVec3 & point, NxVec3 & cp) const;
		NxReal toiVertexTrigAngular(const DereferencedTriangle &, const NxVec3 & point, NxVec3 & cp, NxQuat & iq, CCDContactMode mode) const;
		NxReal toiEdgeLinearFat (const DereferencedEdge & staticEdge, const NxVec3 & e0, const NxVec3 & e1, NxVec3 & normal, NxVec3 & cp);
		NxReal toiEdgeLinear (const DereferencedEdge & staticEdge, const NxVec3 & e0, const NxVec3 & e1, NxVec3 & normal, NxVec3 & cp);
		NxReal toiEdgeLinearFAST (const DereferencedEdge & staticEdge, const NxVec3 & e0, const NxVec3 & e1, NxVec3 & normal, NxVec3 & cp);
		NxReal toiEdgeAngular(const DereferencedEdge & staticEdge, const NxVec3 & e0, const NxVec3 & e1, NxVec3 & normal, NxVec3 & cp);
		NxReal toiEdgeAngularFAST(const DereferencedEdge & staticEdge, const NxVec3 & e0, const NxVec3 & e1, NxVec3 & normal, NxVec3 & cp);

		NxReal toiVertexTrigLinearFAST (const DereferencedTriangle &, const NxVec3 & point, NxVec3 & cp) const;
		NxReal toiVertexTrigAngularFAST(const DereferencedTriangle &, const NxVec3 & point, NxVec3 & ip) const;
		NxReal toiEdgeLinearFAST (const DereferencedEdge & staticEdge, const NxVec3 & e0, const NxVec3 & e1);
		NxReal toiEdgeAngularFAST(const DereferencedEdge & staticEdge, const NxVec3 & e0, const NxVec3 & e1);


		//degenerate cases:
		// These tree methods are only used once by toiEdgeAngular, so they could be re-inserted. They could also be replaced by more general ones.
		void calcDistPointsSkew(const NxVec3 & p0, const NxVec3 & dir0, const NxVec3 & p1, const NxVec3 & dir1, NxVec3 & dp0, NxVec3 & dp1);
		NX_BOOL calcQuat(const NxVec3 & axisA, const NxVec3 & axisB, const NxVec3 & dirA, const NxVec3 & dirB, NxQuat & quat);
		NxU32 quadrEq(const NxVec3 & p, const NxVec3 & dir, NxReal distSqr, NxReal slope, NxVec3 & ip0, NxVec3 & ip1);
	public:
		NxVec3 linMotion, angMotion, pivot, angDir, linDir;
		NxReal linMotionMagnitude, angMotionMagnitude;
		NxVec3 contactPoint;
		NxVec3 contactNormal;		//convention: should point toward dynamic object, so that positive push of dynamic separates.
		CCDSkeleton dynamicMesh;	//TODO: currently these have each their memory buffer but we could have them share....
		CCDSkeleton staticMesh;
*/
	
	};



#endif
