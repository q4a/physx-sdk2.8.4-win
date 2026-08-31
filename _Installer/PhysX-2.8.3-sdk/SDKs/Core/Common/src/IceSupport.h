// Temp file used to compile various ICE files - don't touch!
#ifndef NX_ICESUPPORT
#define NX_ICESUPPORT

	#define MESHMERIZER_API

	inline_ BOOL IsAlmostZero(const Point& v)
	{
		if(fabsf(v.x)>1e-6 || fabsf(v.y)>1e-6 || fabsf(v.z)>1e-6)	return FALSE;
		return TRUE;
	}

	inline_ void Multiply3x3(Point& dest, const Point& source, const Matrix4x4& rot)
	{
		dest.x = source.x * rot.m[0][0] + source.y * rot.m[1][0] + source.z * rot.m[2][0];
		dest.y = source.x * rot.m[0][1] + source.y * rot.m[1][1] + source.z * rot.m[2][1];
		dest.z = source.x * rot.m[0][2] + source.y * rot.m[1][2] + source.z * rot.m[2][2];
	}

	inline_ void InverseMultiply3x3(Point& dest, const Point& source, const Matrix4x4& rot)
	{
		dest.x = source.x * rot.m[0][0] + source.y * rot.m[0][1] + source.z * rot.m[0][2];
		dest.y = source.x * rot.m[1][0] + source.y * rot.m[1][1] + source.z * rot.m[1][2];
		dest.z = source.x * rot.m[2][0] + source.y * rot.m[2][1] + source.z * rot.m[2][2];
	}

	//! A function to access a global 4x4 identity matrix
	/*FUNCTION ICEMATHS_API*/ const Matrix4x4* Get4x4IdentityMatrix();
	/*FUNCTION ICEMATHS_API*/ const Matrix3x3* Get3x3IdentityMatrix();

	inline_ float TransformPoint(udword coord, const Point& source, const Matrix4x4& rot)
	{
		return rot.m[3][coord] + source.x * rot.m[0][coord] + source.y * rot.m[1][coord] + source.z * rot.m[2][coord];
	}
	class /*ICEMATHS_API*/ Vertices : public Container
	{
		public:
		// Constructor / Destructor
								Vertices()								{}
								Vertices(udword nb, const Point* verts)	{ Add(&verts->x, nb*(sizeof(Point)/sizeof(float)));	}
								~Vertices()								{}

		inline_	udword			GetNbVertices()					const	{ return GetNbEntries()/3;							}
		inline_	const Point*	GetVertices()					const	{ return (const Point*)GetEntries();				}

				Vertices&		AddVertex(const Point& p)				{ Add(p.x).Add(p.y).Add(p.z);	return *this;		}
				Vertices&		AddVertex(float x, float y, float z)	{ Add(x).Add(y).Add(z);			return *this;		}
	};

	class /*ICEMATHS_API*/ Planes : public Container
	{
		public:
		// Constructor / Destructor
								Planes()								{}
								Planes(udword nb, const Plane* p)		{ Add(&p->n.x, nb*(sizeof(Plane)/sizeof(float)));				}
								~Planes()								{}

		inline_	udword			GetNbPlanes()					const	{ return GetNbEntries()>>2;										}
		inline_	const Plane*	GetPlanes()						const	{ return (const Plane*)GetEntries();							}

		inline_	void			ResetPlanes()							{ Reset();														}

				Planes&			AddPlane(const Plane& p)				{ Add(p.n.x).Add(p.n.y).Add(p.n.z).Add(p.d);	return *this;	}
	};

	class FIFOStack : public Container
	{
		public:
		//! Constructor
								FIFOStack() : mCurIndex(0)	{}
		//! Destructor
								~FIFOStack()				{}
		// Management
		inline_	FIFOStack&		Push(udword entry)			{	Add(entry);	return *this;	}
				bool			Pop(udword &entry);
		private:
				udword			mCurIndex;			//!< Current index within the container
	};

	class LIFOStack : public Container
	{
		public:
		//! Constructor
								LIFOStack()					{}
		//! Destructor
								~LIFOStack()				{}
		// Management
		inline_	LIFOStack&		Push(udword entry)			{	Add(entry);	return *this;	}
				bool			Pop(udword &entry);
	};

	//! This minimal interface is used to link one data structure to another in a unified way.
	struct /*MESHMERIZER_API*/ SurfaceInterface
	{
		inline_ SurfaceInterface()
			: mNbVerts	(0),
			mVerts		(null),
			mNbFaces	(0),
			mDFaces		(null),
			mWFaces		(null)
		{}

		inline_ SurfaceInterface(
			udword			nb_verts,
			const Point*	verts,
			udword			nb_faces,
			const udword*	dfaces,
			const uword*	wfaces
			)
			: mNbVerts	(nb_verts),
			mVerts		(verts),
			mNbFaces	(nb_faces),
			mDFaces		(dfaces),
			mWFaces		(wfaces)
		{}

		udword			mNbVerts;	//!< Number of vertices
		const Point*	mVerts;		//!< List of vertices
		udword			mNbFaces;	//!< Number of faces
		const udword*	mDFaces;	//!< List of faces (dword indices)
		const uword*	mWFaces;	//!< List of faces (word indices)
	};

	void /*IceMaths::*/ComputePassageMatrices(Matrix4x4* mat0to1, Matrix4x4* mat1to0, const Matrix4x4* mat0, const Matrix4x4* mat1);

	//! Computes the AABB around a set of vertices
	inline_ void ComputeAABB(AABB& aabb, const Point* list, udword nb_pts)
	{
		if(list)
		{
			Point Maxi(MIN_FLOAT, MIN_FLOAT, MIN_FLOAT);
			Point Mini(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
			while(nb_pts--)
			{
//				_prefetch(list+1);	// off by one ?
				ComputeMinMax(*list++, Mini, Maxi);
			}
			aabb.SetMinMax(Mini, Maxi);
		}
	}

//#define ICE_HUFF
#ifdef ICE_HUFF
	struct HuffNode
	{
		inline_	HuffNode() : mCount(INVALID_ID), mChild0(INVALID_ID), mChild1(INVALID_ID)	{}

		udword	mCount;
		sdword	mChild0;
		sdword	mChild1;
	};

	struct HuffCode
	{
		inline_	HuffCode() : mCode(INVALID_ID), mCodeNbBits(INVALID_ID)	{}

		udword	mCode;
		sdword	mCodeNbBits;
	};

	class Huffman0
	{
		public:
							Huffman0();
							~Huffman0();

				void		CountBytes(const ubyte* input, udword nb_bytes);
				void		ScaleCounts();
				sdword		BuildTree();
				void		_ConvertTreeToCode(udword code_so_far, sdword bits, sdword node);

				bool		Compress(const ubyte* buffer, udword size, CustomArray& packed);
				void		Decompress(const CustomArray& source, CustomArray& dest);

				udword*		mCounts;
				HuffNode*	mNodes;
				HuffCode*	mCodes;
		private:
				void		CompressData(const ubyte* buffer, udword size, CustomArray& packed);
				void		ExpandData(const CustomArray& source, CustomArray& dest, sdword root_node);
				void		OutputCounts(CustomArray& packed);
				void		InputCounts(const CustomArray& source);
	};
#endif
	void CreateOBB(OBB& dest, const OBB& box, const Point& dir, float d);

#endif
	
