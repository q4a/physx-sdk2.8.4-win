// SUPPORT FILE FROM ICE, NEEDED FOR CONVEX-CONVEX - DO NOT TOUCH !

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
#ifndef ICECONVEXHULLBUILDER_H
#define ICECONVEXHULLBUILDER_H

	class MESHMERIZER_API ConvexHullBuilder : public Allocateable
	{
		public:
		// Constructor/Destructor
											ConvexHullBuilder(ConvexHull* hull);
		virtual								~ConvexHullBuilder();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the convex hull in 3D.
		 *	After the call, get results with the appropriate accessors.
		 *	\param		create	[in] creation structure
		 *	\return		true if success.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual			bool				Compute(const CONVEXHULLCREATE& create);

		virtual			bool				Init(const SurfaceInterface& surface, const udword* nb_polygons=null, const udword* polygon_data=null, const bool* internal=null);

		virtual			bool				Save(Stream& stream)	const;
//		protected:
						ConvexHull*			mHull;
						NxU16				mUnquantizedNormals;

						bool				CreateEdgeList();
						bool				CreateNormals();
						bool				CreatePolygonData();
						bool				CheckHullPolygons()	const;
						bool				CreateTrianglesFromPolygons();

		inline_			udword				ComputeNbPolygons()		const
											{
												if(!mHull->mData.mNbPolygons)	const_cast<ConvexHullBuilder* const>(this)->CreatePolygonData();	// "mutable method"
												return mHull->mData.mNbPolygons;
											}
		inline_	const	PxHullPolygonData&	ComputePolygon(udword i)	const
											{
												if(!mHull->mData.mPolygons)	const_cast<ConvexHullBuilder* const>(this)->CreatePolygonData();	// "mutable method"
												return mHull->mData.mPolygons[i];
											}
/*		inline_	const	EdgeList*			GetEdgeList()			const
											{
												if(!mEdgeList)	const_cast<ConvexHull* const>(this)->CreateEdgeList();	// "mutable method"
												return mEdgeList;
											}*/

		inline_	udword						ComputeNbEdges()		const
											{
												if(!mHull->mData.mNbEdges)	const_cast<ConvexHullBuilder* const>(this)->CreateEdgeList();	// "mutable method"
												return mHull->mData.mNbEdges;
											}
//		inline_	const NxPhysics::Edge*		ComputeEdges()			const
		inline_	const PxEdge8Data*			ComputeEdges()			const
											{
												if(!mHull->mData.mEdges8)	const_cast<ConvexHullBuilder* const>(this)->CreateEdgeList();	// "mutable method"
												return mHull->mData.mEdges8;
											}
		inline_	const Point*				ComputeEdgeNormals()	const
											{
												if(!mHull->mData.mEdgeNormals)	const_cast<ConvexHullBuilder* const>(this)->CreateEdgeList();	// "mutable method"
												return mHull->GetEdgeNormals();
											}
		inline_	const PxEdgeDescData*		ComputeEdgeToTriangles()	const
											{
												if(!mHull->mData.mEdgeToTriangles)	const_cast<ConvexHullBuilder* const>(this)->CreateEdgeList();	// "mutable method"
												return mHull->mData.mEdgeToTriangles;
											}
//		inline_	const udword*				ComputeFacesByEdges()		const
		inline_	const ubyte*				ComputeFacesByEdges()		const
											{
												if(!mHull->mData.mFacesByEdges8)	const_cast<ConvexHullBuilder* const>(this)->CreateEdgeList();	// "mutable method"
												return mHull->mData.mFacesByEdges8;
											}

		inline_	const Point*				ComputeVertexNormals()		const
											{
												if(!mHull->mData.mHullNormals)	const_cast<ConvexHullBuilder* const>(this)->CreateNormals();	// "mutable method"
												return mHull->GetVertexNormals();
											}
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *	Analyses a convex hull made of triangles and extracts polygon data out of it.
	 *	\relates	ConvexHull
	 *	\fn			ExtractHullPolygons(Container& polygon_data, const ConvexHull& hull)
	 *	\param		nb_polygons		[out] number of extracted polygons
	 *	\param		polygon_data	[out] polygon data: (Nb indices, index 0, index 1... index N)(Nb indices, index 0, index 1... index N)(...)
	 *	\param		hull			[in] convex hull
	 *	\return		true if success
	 */
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FUNCTION MESHMERIZER_API bool ExtractHullPolygons(udword& nb_polygons, Container& polygon_data, const ConvexHull& hull, Container* triangle_data=null);

#endif	// ICECONVEXHULLBUILDER_H

