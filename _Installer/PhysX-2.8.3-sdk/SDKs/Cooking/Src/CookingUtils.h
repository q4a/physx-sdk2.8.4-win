// Temp file used to compile various ICE files - don't touch!
#ifndef NX_COOKINGUTILS
#define NX_COOKINGUTILS

	/*FUNCTION ICECORE_API*/ bool	FindLineStrip(Container& line_strip, const Container& line_segments);
//	/*FUNCTION ICECORE_API*/ bool	InverseBuffer(udword nb_entries, udword* entries);
	/*FUNCTION ICECORE_API*/ bool	InverseBuffer(udword nb_entries, ubyte* entries);

	//! Vertex cloud reduction result structure
	struct /*MESHMERIZER_API*/ REDUCEDCLOUD
	{
				// Out
				Point*				RVerts;		//!< Reduced list
				udword				NbRVerts;	//!< Reduced number of vertices
				udword*				CrossRef;	//!< nb_verts remapped indices
	};

	class /*MESHMERIZER_API*/ ReducedVertexCloud
	{
		public:
		// Constructors/destructor
//									ReducedVertexCloud(const VertexCloud* vcloud);
									ReducedVertexCloud(const Point* verts, udword nb_verts);
									~ReducedVertexCloud();
		// Free used bytes
				ReducedVertexCloud&	Clean();
		// Cloud reduction
				bool				Reduce(REDUCEDCLOUD* rc=null);
				bool				Reduce2(REDUCEDCLOUD* rc=null);
		// Data access
		inline_	udword				GetNbVerts()				const	{ return mNbVerts;		}
		inline_	udword				GetNbReducedVerts()			const	{ return mNbRVerts;		}
		inline_	const Point*		GetReducedVerts()			const	{ return mRVerts;		}
		inline_	const Point&		GetReducedVertex(udword i)	const	{ return mRVerts[i];	}
		inline_	const udword*		GetCrossRefTable()			const	{ return mXRef;			}

		private:
		// Original vertex cloud
				udword				mNbVerts;	//!< Number of vertices
				const Point*		mVerts;		//!< List of vertices (pointer copy)

		// Reduced vertex cloud
				udword				mNbRVerts;	//!< Reduced number of vertices
				Point*				mRVerts;	//!< Reduced list of vertices
				udword*				mXRef;		//!< Cross-reference table (used to remap topologies)
	};

	//! Normals creation structure
	struct /*MESHMERIZER_API*/ NORMALSCREATE{
		//! Constructor
						NORMALSCREATE()	{ ZeroMemory(this, SIZEOFOBJECT); }
		// In
		udword			NbVerts;		//!< Number of vertices
		const Point*	Verts;			//!< List of vertices
		udword			NbFaces;		//!< Number of faces
		const udword*	dFaces;			//!< List of faces (triangle list) or null
		const uword*	wFaces;			//!< List of faces (triangle list) or null
		bool			UseAngles;		//!< Take edge angles into account
		// Out
		Point*			FNormals;		//!< Face-normals. If available, normals are written here, else kept in the class.
		Point*			VNormals;		//!< Vertex-normals. If available, normals are written here, else kept in the class.
	};

	class /*MESHMERIZER_API*/ SmoothNormals
	{
		public:
		// Constructor/Destructor
						SmoothNormals();
						~SmoothNormals();

		// Computes normals.
				bool	Compute(const NORMALSCREATE& nc);

		// Data access
		inline_	Point*	GetFaceNormals()	const	{ return mFNormals; }
		inline_	Point*	GetVertexNormals()	const	{ return mVNormals; }

		private:
				Point*	mFNormals;
				Point*	mVNormals;
	};

#endif
	
