// SUPPORT FILE FROM ICE, NEEDED FOR CONVEX-CONVEX - DO NOT TOUCH !

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICECONVEXDECOMPOSER_H
#define ICECONVEXDECOMPOSER_H

#ifndef NX_COOKING
#error Do not include anymore!
#endif

//namespace NxPhysics
	struct DecompParams
	{
		const IceMaths::Plane*	mPlanes;
		const IndexedTriangle*	mFaces;
		const Point*			mVerts;
		const NxPhysics::EdgeList*			mEdgeList;
		Container*				mConnectedTriangles;
		Container*				mConnectedVertices;
		const bool*				mEdgeFlags;
		udword*					mTags;
		udword					mID;
		udword					mNb;
	};

	class ConvexDecomposer
	{
		public:
		// Constructor / destructor
										ConvexDecomposer();
										~ConvexDecomposer();

				bool					Decompose(udword nb_faces, const IndexedTriangle* faces, udword nb_verts, const Point* verts/*, const NxPhysics::EdgeList& list*/);

		//! These calls are only valid after ExtractSubMeshes() has been called
		inline_	udword					GetNbConvexSubmeshes()	const	{ return mConvexID;		}
		inline_	const udword*			GetConvexTags()			const	{ return mConvexTags;	}
		inline_	udword					GetNbFlatSubmeshes()	const	{ return mFlatID;		}
		inline_	const udword*			GetFlatTags()			const	{ return mFlatTags;		}

		private:
				DecompParams			mParams;
//				udword					mNbConnectedFaces;		//!< Number of connected faces for current submesh
				udword					mConvexID;				//!< Submesh ID
				udword*					mConvexTags;			//!< One tag/face, each face is marked with a submesh ID
				udword					mFlatID;				//!< Submesh ID
				udword*					mFlatTags;				//!< One tag/face, each face is marked with a submesh ID
		// Internal methods
				bool					ExtractSubmeshFromFace(/*DecompParams& params,*/udword new_tri, udword nb_faces);
				void					_TrackTriangle(/*DecompParams& params,*/ udword new_tri, udword new_vertex);
				void					ProcessPatch(udword nb_indices, const udword* indices, const Point* faceNormals, const IndexedTriangle* faces, const Point* verts);
				void					MakeRelativePatch(udword nb_indices, const udword* indices);

void	NonRecursiveTrack(udword new_tri, udword nb_faces);
void	_TrackTriangle(FIFOStack& stack, udword new_tri, udword new_vertex);

	};

#endif	// ICECONVEXDECOMPOSER_H
