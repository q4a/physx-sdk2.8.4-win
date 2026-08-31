///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for hull-related Gauss maps
 *	\file		IceHullGaussMaps.h
 *	\author		Pierre Terdiman
 *	\date		September, 12, 2004
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEHULLGAUSSMAPS_H
#define ICEHULLGAUSSMAPS_H

#include "px_hullgaussmaps.h"

	class MESHMERIZER_API ClosestPolygonMap : public GaussMap
	{
		public:
								ClosestPolygonMap(const ConvexHull* hull);
		virtual					~ClosestPolygonMap();

				ubyte*			mSamples;
		private:
		const	ConvexHull*		mHull;

		friend class ClosestPolygonMapBuilder;
	};

	class MESHMERIZER_API SupportVertexMap : public GaussMap
	{
		public:
								SupportVertexMap(const ConvexHull* hull);
		virtual					~SupportVertexMap();

		virtual	bool			Load(const Stream& stream);

				//ubyte*			mSamples;
				//ubyte*			mSamples2;
				ubyte*					mBuffer;
				PxSupportVertexMapData	mData;
		private:
		const	ConvexHull*		mHull;
		friend class SupportVertexMapBuilder;
	};

	class MESHMERIZER_API RaycastMap : public GaussMap
	{
		public:
								RaycastMap(const ConvexHull* hull);
		virtual					~RaycastMap();

		virtual	bool			Load(const Stream& stream);

		// Experimental
				bool			FastPointCulling(const Point& p)	const;
				bool			PointInHull(const Point& p)			const;

				ubyte*			mSamples;
		private:
		const	ConvexHull*		mHull;
		friend class RaycastMapBuilder;
	};

#ifdef MSH_SUPPORT_PROJ_MAP
	class MESHMERIZER_API ProjectionMap : public GaussMap
	{
		public:
								ProjectionMap(const CullingHull* hull);
		virtual					~ProjectionMap();

		// Implements GaussMap
		virtual	bool			Initialize();
		virtual	bool			PrecomputeSample(udword offset, const Point& dir);

				float*			mMinSamples;
				float*			mMaxSamples;
		private:
		const	CullingHull*	mHull;
	};
#endif

#endif // ICEHULLGAUSSMAPS_H
