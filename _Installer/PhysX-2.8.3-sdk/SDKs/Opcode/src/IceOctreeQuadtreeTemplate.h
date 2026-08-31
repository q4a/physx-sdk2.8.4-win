

#ifdef USE_QUADTREE
	#ifdef USE_OCTREE
		#error Define only one of them!
	#endif
#endif

#ifdef USE_QUADTREE
	#pragma message("Using quadtree")
#endif
#ifdef USE_OCTREE
	#pragma message("Using octree")
#endif

#ifdef USE_QUADTREE
	class Point2D
	{
		public:

		//! Empty constructor
		inline_					Point2D()										{}
		//! Constructor from floats
		inline_					Point2D(float _x, float _y) : x(_x), y(_y)		{}
		//! Copy constructor
		inline_					Point2D(const Point2D& p) : x(p.x), y(p.y)		{}
		//! Destructor
		inline_					~Point2D()										{}

		inline_	float			Max()								const		{ return MAX(x, y);							}
		inline_	Point2D&		Neg()											{ x = -x;		y = -y;		return *this;	}
		inline_	Point2D&		Zero()											{ x =			y =	0.0f;	return *this;	}

		//! Operator for Point2D Plus = Point2D + Point2D.
		inline_	Point2D			operator+(const Point2D& p)			const		{ return Point2D(x + p.x, y + p.y);		}
		//! Operator for Point2D Minus = Point2D - Point2D.
		inline_	Point2D			operator-(const Point2D& p)			const		{ return Point2D(x - p.x, y - p.y);		}

		//! Operator for Point2D Scale = Point2D * float.
		inline_	Point2D			operator*(float s)					const		{ return Point2D(x * s,   y * s);		}
		//! Operator for Point2D Scale = float * Point2D.
		inline_ friend	Point2D	operator*(float s, const Point2D& p)			{ return Point2D(s * p.x, s * p.y);		}

		inline_					operator	const	float*() const	{ return &x; }
		inline_					operator			float*()		{ return &x; }

		float x,y;
	};

	class AABB_2D
	{
		public:

		//! Get min point of the box
		inline_			void		GetMin(Point2D& min)					const		{ min = mMin;								}
		//! Get max point of the box
		inline_			void		GetMax(Point2D& max)					const		{ max = mMax;								}

		//! Get component of the box's min point along a given axis
		inline_			float		GetMin(udword axis)						const		{ return mMin[axis];						}
		//! Get component of the box's max point along a given axis
		inline_			float		GetMax(udword axis)						const		{ return mMax[axis];						}

		//! Get box center
		inline_			void		GetCenter(Point2D& center)				const		{ center = (mMax + mMin)*0.5f;				}
		//! Get box extents
		inline_			void		GetExtents(Point2D& extents)			const		{ extents = (mMax - mMin)*0.5f;				}
		//! Get component of the box's center along a given axis

		inline_			float		GetCenter(udword axis)					const		{ return (mMax[axis] + mMin[axis])*0.5f;	}
		//! Get component of the box's extents along a given axis
		inline_			float		GetExtents(udword axis)					const		{ return (mMax[axis] - mMin[axis])*0.5f;	}

						void		SetMinMax(const Point2D& min, const Point2D& max)	{ mMin = min;		mMax = max;				}
						void		SetCenterExtents(const Point2D& c, const Point2D& e){ mMin = c - e;		mMax = c + e;			}

		//! Operator for AABB *= float. Scales the extents, keeps same center.
		inline_			AABB_2D&	operator*=(float s)
									{
										Point2D Center;		GetCenter(Center);
										Point2D Extents;	GetExtents(Extents);
										SetCenterExtents(Center, Extents * s);
										return *this;
									}

						float		MakeCube(AABB_2D& cube) const
									{
										Point2D Ext;	GetExtents(Ext);
										float Max = Ext.Max();

										Point2D Cnt;	GetCenter(Cnt);
										cube.SetCenterExtents(Cnt, Point2D(Max, Max));
										return Max;
									}

		inline_	BOOL				ContainsPoint(const Point2D& p)		const
									{
										if(p.x > GetMax(0) || p.x < GetMin(0)) return FALSE;
										if(p.y > GetMax(1) || p.y < GetMin(1)) return FALSE;
										return TRUE;
									}

		inline_			BOOL		Intersect(const AABB_2D& a)				const
									{
										if(mMax.x < a.mMin.x
										|| a.mMax.x < mMin.x
										|| mMax.y < a.mMin.y
										|| a.mMax.y < mMin.y)	return FALSE;

										return TRUE;
									}

		Point2D	mMin;
		Point2D	mMax;
	};

	class Sphere_2D
	{
		public:

		Point2D	mCenter;
		float	mRadius;
	};
#endif

#ifdef USE_QUADTREE
	typedef	AABB_2D		OctreeBox;
	typedef	Sphere_2D	OctreeSphere;
	typedef	Point2D		OctreePoint;
#else
	typedef	AABB		OctreeBox;
	typedef	Sphere		OctreeSphere;
	typedef	Point		OctreePoint;
#endif

#ifdef USE_QUADTREE
	#define NB_OCTREE_CHILDREN	4
	#define NB_OCTREE_SHIFT		2
	#define CELL_CLASS_NAME		QuadtreeCell
	#define TREE_CLASS_NAME		LinearLooseQuadtree
	#define CALLBACK_NAME		QuadtreeCallback
	#define PRUNER_NAME			QuadtreePruner
#else
	#define NB_OCTREE_CHILDREN	8
	#define NB_OCTREE_SHIFT		3
	#define CELL_CLASS_NAME		OctreeCell
	#define TREE_CLASS_NAME		LinearLooseOctree
	#define CALLBACK_NAME		OctreeCallback
	#define PRUNER_NAME			OctreePruner
#endif

	#define MAKE_CHILD_INDEX(parent_index, child_number)	((parent_index<<NB_OCTREE_SHIFT) + child_number)
	#define MAKE_PARENT_INDEX(current_index)				current_index--;	current_index>>=NB_OCTREE_SHIFT;

	class OPCODE_API CELL_CLASS_NAME : public Allocateable
	{
		public:
		inline_						CELL_CLASS_NAME() : mCount(0), mList(null)	{}
		inline_						~CELL_CLASS_NAME()							{}

#ifndef USE_IMPLICIT_BOX
		inline_	const OctreeBox&	GetBox()				const	{ return mBox;		}
				void				ScaleBox(float scale)			{ mBox*=scale;		}
#endif
		inline_	udword				GetCount()				const	{ return mCount;	}
		inline_	Opcode::Prunable*	GetObjects()			const	{ return mList;		}

		inline_	void				Dump(ContainerSizeT& list)	const
									{
										Opcode::Prunable* Obj = mList;
										while(Obj)
										{
											list.Add(size_t(Obj));
											Obj = Obj->GetNextObject();
										}
									}
		private:
#ifndef USE_IMPLICIT_BOX
				OctreeBox			mBox;		//!< Cell's AABB
#endif
				udword				mCount;		//!< Total number of objects in that cell AND children
				Opcode::Prunable*	mList;		//!< Linked list of objects contained in that cell

		friend	class				TREE_CLASS_NAME;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *	User-callback, called for each octree cell by the walking code.
	 *	\param		parent		[in] parent cell
	 *	\param		current		[in] current cell
	 *	\param		user_data	[in] user-defined data
	 *	\return		true to recurse through children, else false to bypass them
	 */
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef bool (*CALLBACK_NAME)	(CELL_CLASS_NAME* parent, CELL_CLASS_NAME* current, void* user_data);

	class OPCODE_API TREE_CLASS_NAME : public Allocateable
	{
		public:
		// Constructor/Destructor
									TREE_CLASS_NAME();
									~TREE_CLASS_NAME();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Initializes the loose octree.
		 *	\param		max_depth	[in] maximum depth for the linear octree
		 *	\param		world_box	[in] world's maximum bounding box
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_QUADTREE
				bool				Init(udword max_depth, const OctreeBox& world_box, udword quadtree_up);
#else
				bool				Init(udword max_depth, const OctreeBox& world_box);
#endif

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Releases everything.
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool				Release();

		inline_	bool				InsertObject(Opcode::Prunable& obj)		{ return InsertObject(obj, null);	}
				bool				RemoveObject(Opcode::Prunable& obj);
				bool				UpdateObject(Opcode::Prunable& obj);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Traverses the whole octree, calling the user back for each cell.
		 *	\param		cb			[in] user-defined callback
		 *	\param		user_data	[in] user-defined data
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool				Walk(CALLBACK_NAME cb, void* user_data)	const;

		// Data access
		inline_	udword				GetMaxDepth()				const	{ return mMaxDepth;						}
		inline_	udword				GetNbCells()				const	{ return mNbCells;						}
		inline_	CELL_CLASS_NAME*	GetCells()					const	{ return mCells;						}
		inline	const OctreeBox&	GetWorldBox()				const	{ return mWorldBox;						}
		inline_	udword				GetNbContainedObjects()		const	{ return mCells ? mCells[0].mCount : 0;	}
#ifdef USE_QUADTREE
		inline_	udword				GetQuadtreeUp()				const	{ return mQuadtreeUp;					}
#endif
				void				TestAgainstPlanes(const Plane* planes, udword nb_planes, ContainerSizeT& box_indices_clip, ContainerSizeT& box_indices_noclip);
				void				TestAgainstSegment(const Segment& segment, ContainerSizeT& candidates);
				void				TestAgainstRay(const Ray& ray, ContainerSizeT& candidates);
				void				TestAgainstSphere(const Sphere& sphere, ContainerSizeT& candidates, ContainerSizeT& touched);
				void				TestAgainstCapsule(const LSS& capsule, ContainerSizeT& candidates, ContainerSizeT& touched);
				void				TestAgainstAABB(const AABB& aabb, ContainerSizeT& candidates, ContainerSizeT& touched);

#ifdef USE_IMPLICIT_BOX
				void				ComputeBox(udword index, OctreeBox& box)	const;
				void				ComputeBox(udword index, OctreePoint& center, float& extent) const;
				udword				ComputeChildrenBoxes(udword parent_index, OctreePoint* centers, float& extent, udword* indices) const;
				void				ComputeOrder(const Point& dir, udword* sorted)	const;
#endif
		private:
		// World data
				OctreeBox			mWorldBox;		//!< Original world box used to build the octree
				OctreePoint			mWorldOffset;
				float				mWorldSize;		//!< Original cube size (not loose)
				float				mWorldSize2;	//!< Precomputed mWorldSize/2
/*				float				mWorldSize4;	//!< Precomputed mWorldSize/4
				float				mWorldSize8;	//!< Precomputed mWorldSize/8
				float				mWorldSize16;	//!< Precomputed mWorldSize/16
				float				mWorldSize32;	//!< Precomputed mWorldSize/32
				float				mWorldSize64;	//!< Precomputed mWorldSize/64
				float				mWorldSize128;	//!< Precomputed mWorldSize/128*/
#ifdef USE_QUADTREE
				udword				mQuadtreeUp;
#endif
		// Tree data
				udword				mMaxDepth;		//!< Maximum octree depth
				udword				mNbCells;		//!< Total number of cells in the linear octree = f(mMaxDepth)
				CELL_CLASS_NAME*	mCells;			//!< Linear list of mNbCells cells
		// Internal methods
				void				_Walk(udword index, CELL_CLASS_NAME* parent, CALLBACK_NAME cb, void* user_data)	const;
				bool				InsertObject(Opcode::Prunable& obj, const OctreeBox* bounding_cube);
				void				_InitOctree(udword index, const OctreeBox& box);
	};


