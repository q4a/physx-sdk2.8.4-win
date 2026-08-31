#ifndef NX_FOUNDATION_DEBUGRENDERABLE
#define NX_FOUNDATION_DEBUGRENDERABLE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "NxDebugRenderable.h"
#include "Allocateable.h"
#include "Array.h"


/**
Default implementation of NxDebugRenderable class.
*/
namespace NxFoundation
	{
typedef NxArraySDK<NxDebugPoint> PointArray;
typedef NxArraySDK<NxDebugLine> LineArray;
typedef NxArraySDK<NxDebugTriangle> TrisArray;

class DebugRenderable : public NxAllocateable
	{
	public:
	DebugRenderable();
	~DebugRenderable();

	NxDebugRenderable getRenderable();

	NxU32 getNbPoints() const;
	const NxDebugPoint* getPoints() const;
	NxU32 getNbLines() const;
	const NxDebugLine* getLines() const;
	NxU32 getNbTriangles() const;
	const NxDebugTriangle* getTriangles() const;
	void addPoint(const NxVec3& p, NxU32 color);
	void addLine(const NxVec3& p0, const NxVec3& p1, NxU32 color);
	void addTriangle(const NxVec3& p0, const NxVec3& p1, const NxVec3& p2, NxU32 color);
	void clear();
	void addOBB(const NxBox& box, NxU32 color=0xffffffff, bool renderFrame=false);
	void addAABB(const NxBounds3& bounds, NxU32 color=0x00ffff00, bool renderFrame=false);
	void addArrow(const NxVec3& position, const NxVec3& direction, NxReal length, NxReal scale, NxU32 color=0xffffffff);
	void addBasis(const NxVec3& position, const NxMat33& columns, const NxVec3& lengths, NxReal scale, NxU32 colors[3] = 0);
	void addCircle(NxU32 nbSegments, const NxMat34& matrix, NxU32 color, NxF32 radius, bool semicircle = false);

	void appendRenderable(const NxDebugRenderable & other);

	private:
	PointArray	pointsArray;
	LineArray	linesArray;
	TrisArray	trianglesArray;
	};

	}
#endif
