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
#ifndef ICEHULLGAUSSMAPSBUILDER_H
#define ICEHULLGAUSSMAPSBUILDER_H

#include "IceGaussMapBuilder.h"

	class MESHMERIZER_API ClosestPolygonMapBuilder : public GaussMapBuilder
	{
		public:
								ClosestPolygonMapBuilder(ClosestPolygonMap* gm);
		virtual					~ClosestPolygonMapBuilder();

		// Implements GaussMap
		virtual	bool			Initialize();
		virtual	bool			PrecomputeSample(udword offset, const Point& dir);
		virtual	void			PostInit();

				ClosestPolygonMap*	mCPM;
	};

	class MESHMERIZER_API SupportVertexMapBuilder : public GaussMapBuilder
	{
		public:
								SupportVertexMapBuilder(SupportVertexMap* gm);
		virtual					~SupportVertexMapBuilder();

		// Implements GaussMap
		virtual	bool			Initialize();
		virtual	bool			PrecomputeSample(udword offset, const Point& dir);
		virtual	void			PostInit();

		virtual	bool			Save(Stream& stream)	const;

				SupportVertexMap*	mSVM;
	};

	class MESHMERIZER_API RaycastMapBuilder : public GaussMapBuilder
	{
		public:
								RaycastMapBuilder(RaycastMap* gm);
		virtual					~RaycastMapBuilder();

		// Implements GaussMap
		virtual	bool			Initialize();
		virtual	bool			PrecomputeSample(udword offset, const Point& dir);
		virtual	void			PostInit();

		virtual	bool			Save(Stream& stream)	const;

				RaycastMap*		mRM;
	};

#endif // ICEHULLGAUSSMAPSBUILDER_H
