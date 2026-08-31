// SUPPORT FILE FROM ICE, NEEDED FOR CONVEX-CONVEX - DO NOT TOUCH !
// touched for split for high/low level.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code to compute a convex hull in 3D.
 *	\file		IceConvexHull.h
 *	\author		Pierre Terdiman
 *	\date		January, 29, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICECONVEXHULL_H
#define ICECONVEXHULL_H

#include "IceSupport.h"

#include "px_convexhull.h"

#define USE_PRECOMPUTED_HULL_PROJECTION
#define SUPPORT_EDGE_DIRECTIONS
#define USE_HULL_TRIANGLES

//#ifdef USE_HULL_TRIANGLES
//	typedef IndexedTriangle		HullTriangle;	// 32-bit just for Opcode...
//#else
//	typedef IndexedTriangle16	HullTriangle;
//#endif

#ifndef USE_HULL_TRIANGLES
#error "16-bit triangles not supported atm. IndexedTriangle and PxHullTriangleData must be same data-layout!"
#endif
/*
	enum ConvexPolygonFlag
	{
		CPF_INTERNAL	= (1<<0),
	};*/

	//class NxPhysics::Edge8;
	//class NxPhysics::EdgeDesc;
	struct PxEdgeDescData;
	struct PxEdge8Data;

	//! Convex hull creation structure
	struct MESHMERIZER_API CONVEXHULLCREATE
	{
		//! Constructor
						CONVEXHULLCREATE()	{ ZeroMemory(this, SIZEOFOBJECT); }

		udword			NbVerts;		//!< Number of source vertices
		const Point*	Vertices;		//!< List of source vertices
		float			Scale;			//!< Artificial hull scale
		bool			UnifyNormals;	//!< true if hull faces orientation must be unified
		bool			PolygonData;	//!< true to extract polygon data
	};


	class MESHMERIZER_API ConvexHull : public Allocateable
	{
		public:
		// Constructor/Destructor
											ConvexHull();
		virtual								~ConvexHull();

		virtual			bool				Load(const Stream& stream);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Checks a point is inside the hull.
		 *	\param		p	[in] point in local space
		 *	\return		true if the hull contains the point
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						bool				Contains(const Point& p)	const;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Checks a sphere is inside the hull.
		 *	\param		sphere	[in] sphere in local space
		 *	\return		true if the hull contains the sphere
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						bool				Contains(const Sphere& sphere)	const;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the center of the hull. It should be inside it !
		 *	\param		center	[out] hull center
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						bool				ComputeGeomCenter(Point& center) const;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the polygon whose normal is closest to given witness vector.
		 *	\param		world_witness	[in] witness vector in world space
		 *	\param		world			[in] possible world matrix for the hull, or null
		 *	\return		index of closest polygon
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						udword				SelectClosestPolygon(const Point& world_witness, const Matrix4x4* world=null)	const;
						udword				SelectClosestEdge(const Point& world_witness, const Matrix4x4* world=null, udword* code=null)		const;

						udword				DumpPolyNeighbors(udword poly_index, udword* indices) const;

		// Data access
		inline_			udword				GetNbFaces()			const	{ return mData.mNbHullFaces;		}
		inline_	const	PxHullTriangleData*	GetFaces()				const	{ return mData.mFaces;			}
		inline_			udword				GetNbVerts()			const	{ return mData.mNbHullVertices;	}
		inline_	const	Point*				GetVerts()				const	{ return (Point*)mData.mHullVertices;		}
		inline_	const	Point&				GetCenter()				const	{ return (Point&)mData.mCenter;			}

		// PT: super painful: cooking completely changes the way this class works. Before, all data-structures were lazy-evaluated through
		// those accessors. Now this is just not possible anymore, leading to potentially wasted resources.

		inline_			udword				GetNbPolygons()			const	{ return mData.mNbPolygons;		}
		inline_	const	PxHullPolygonData&	GetPolygon(udword i)	const	{ return mData.mPolygons[i];	}
		inline_	const	PxHullPolygonData*	GetPolygons()			const	{ return mData.mPolygons;		}
/*		inline_	const	EdgeList*			GetEdgeList()			const
											{
												if(!mEdgeList)	const_cast<ConvexHull* const>(this)->CreateEdgeList();	// "mutable method"
												return mEdgeList;
											}*/
#ifdef SUPPORT_EDGE_DIRECTIONS
		inline_	const	Vertices*			GetEdgeDirections()		const
											{
												if(!mEdgeDirections)	const_cast<ConvexHull* const>(this)->CreateEdgeDirections();	// "mutable method"
												return mEdgeDirections;
											}
#endif

		inline_	udword						GetNbEdges()			const	{ return mData.mNbEdges;			}
//		inline_	const NxPhysics::Edge*		GetEdges()				const	{ return mEdges;			}
//#ifdef SUPPORT_CONVEX_EDGE_DATA
		inline_	const PxEdge8Data*			GetEdges8()				const	{ return mData.mEdges8;			}
		inline_	const PxEdgeDescData*		GetEdgeToTriangles()	const	{ return mData.mEdgeToTriangles;	}
//		inline_	const udword*				GetFacesByEdges()		const	{ return mFacesByEdges;		}
		inline_	const ubyte*				GetFacesByEdges8()		const	{ return mData.mFacesByEdges8;	}
//#endif
		inline_	const Point*				GetEdgeNormals()		const	{ return (Point*)mData.mEdgeNormals;		}
		inline_	const Point*				GetVertexNormals()		const	{ return (Point*)mData.mHullNormals;		}

//		protected:	// Temp removed to expose hull polygons
						PxConvexHullData	mData;
		
						void*				mHullData;

#ifdef SUPPORT_EDGE_DIRECTIONS
						Vertices*			mEdgeDirections;		//!< Edge directions, UNUSED?
#endif
		// Internal methods
#ifdef SUPPORT_EDGE_DIRECTIONS
						bool				CreateEdgeDirections();
#endif
			friend class ConvexHullBuilder;
	};

#endif	// ICECONVEXHULL_H

