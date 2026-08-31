#ifndef __NP_BUILDSMOOTHNORMALS__
#define __NP_BUILDSMOOTHNORMALS__

bool NX_CALL_CONV NpBuildSmoothNormals(
		NxU32 nbTris,
		NxU32 nbVerts,
		const NxVec3* verts,
		const NxU32* dFaces,
		const NxU16* wFaces,
		NxVec3* normals,
		bool flip=false
		);

#endif
