#ifndef CLOTH_HASH_H
#define CLOTH_HASH_H

#include "Physics.h"
#include "NxFoundation.h"
#include "Array.h"

// ------------------------------------------------------------------------------

struct ClothHashRoot {
	int first;
	NxU32 timeStamp;
};

// ------------------------------------------------------------------------------
struct ClothVertexRef {
	bool operator < (const ClothVertexRef &ref) const {
		if (entityId < ref.entityId) return true;
		if (entityId > ref.entityId) return false;
		return vertexNr < ref.vertexNr;
	}
	bool operator == (const ClothVertexRef &ref) const {
		return entityId == ref.entityId && vertexNr == ref.vertexNr;
	}
	NxU32 entityId;	// cloth or fluid number
	NxU32 vertexNr;
};

// ------------------------------------------------------------------------------
struct ClothSelfContact {
	ClothVertexRef p;	// reference to the vertex that collided with a triangle
	NxU32 vertNr[3];	// indices of triangle vertices (always from the local cloth)
	NxReal sign;		// -1.0f or 1.0f depending on the side of the triangle p should be
};

// ------------------------------------------------------------------------------
enum ClothBoundsState { SCBS_OLD, SCBS_PERSISTENT, SCBS_NEW };

struct ClothBounds {
	bool operator == (const ClothBounds &bounds) {
		return xi == bounds.xi && yi == bounds.yi && zi == bounds.zi;
	}

	NxI32 xi,yi,zi;
	NxU16 state;
	NxI16 lifetime;
};

//----------------------------------------------------------------------------//

struct ClothBoundsHashEntry {
	ClothBounds bounds;
	int next;
};

// ------------------------------------------------------------------------------
struct ClothHashEntry {
	ClothVertexRef ref;
	int next;
};

// ------------------------------------------------------------------------------
class ClothHash {
public: 
	ClothHash();
	~ClothHash();

	void setGridSpacing(NxReal spacing);
	void setTime(NxI32 time)  { mTime = time;}
	NxReal getGridSpacing() { return 1.0f / mInvSpacing; }
	void reset();
	void add(const NxBounds3 &bounds, const ClothVertexRef &vertexRef);
	void query(const NxBounds3 &bounds, NxFoundation::NxArraySDK<ClothVertexRef> &vertexRefs, int maxRefs = -1);
	void queryUnique(const NxBounds3 &bounds, NxFoundation::NxArraySDK<ClothVertexRef> &vertexRefs, int maxRefs = -1);

	void add(const NxVec3 &pos, const ClothVertexRef &vertexRef);
	void query(const NxVec3 &pos, NxFoundation::NxArraySDK<ClothVertexRef> &vertexRefs, int maxRefs = -1);
	void queryUnique(const NxVec3 &pos, NxFoundation::NxArraySDK<ClothVertexRef> &vertexRefs, int maxRefs = -1);

	// for cloth HSM
	void clear();
	void insert(const ClothBounds& bounds);
	ClothBounds* find(const ClothBounds& bounds);
	ClothBounds* insertAndFind(const ClothBounds& bounds, bool &wasAlreadyInserted);
	NxU32 getNumBounds() { return mBoundsEntries.size(); }
	ClothBounds& getBounds(int i) { return mBoundsEntries[i].bounds; }
private:
	void quickSort(NxFoundation::NxArraySDK<ClothVertexRef> &vertexRefs, int l, int r);
	void compressIndices(NxFoundation::NxArraySDK<ClothVertexRef> &vertexRefs);
	static const NxU32 mHashIndexSize = 17011;
	NxReal mSpacing;
	NxReal mInvSpacing;
	NxI32 mTime;

	NX_INLINE NxU32 hashFunction(int xi, int yi, int zi) { 
	    NxU32 h = (xi * 92837111)^(yi * 689287499)^(zi * 283923481);
	    return h % mHashIndexSize;
    }

	NX_INLINE NxU32 hashFunction(const ClothBounds& bounds) { 
		NxU32 h = (bounds.xi * 92837111)^(bounds.yi * 689287499)^(bounds.zi * 283923481);
		return h % mHashIndexSize;
	}

	NX_INLINE void cellCoordOf(const NxVec3 &v, int &xi, int &yi, int &zi) {
		xi = (int)(v.x * mInvSpacing); if (v.x < 0.0f) xi--;
		yi = (int)(v.y * mInvSpacing); if (v.y < 0.0f) yi--;
		zi = (int)(v.z * mInvSpacing); if (v.z < 0.0f) zi--;
	}

	ClothHashRoot mHashIndex[mHashIndexSize];
	NxFoundation::NxArraySDK<ClothHashEntry> mEntries;

	// for cloth HSM
	NxFoundation::NxArraySDK<ClothBoundsHashEntry> mBoundsEntries;
};


#endif
