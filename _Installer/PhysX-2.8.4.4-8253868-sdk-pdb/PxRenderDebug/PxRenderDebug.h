#ifndef __PX_RENDER_DEBUG_H__
#define __PX_RENDER_DEBUG_H__
/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

/*!
\file
\brief debug rendering classes and structures
*/

#include "PsShare.h"
#include "PxVec3.h"
#include "PxQuat.h"
#include "PxMat33Legacy.h"
#include "PxMat44.h"
#include "PxMat34Legacy.h"
#include "PxBounds3.h"

namespace physx
{
	namespace shdfnd2
	{


#pragma PX_PUSH_PACK_DEFAULT

#if defined(PX_PS3)
static const PxU32 MAX_BUFFER_STACK=512; // maximum number of vertices we are allowed to create on the stack.
#elif defined(PX_X360)
static const PxU32 MAX_BUFFER_STACK=512; // maximum number of vertices we are allowed to create on the stack.
#else
static const PxU32 MAX_BUFFER_STACK=8192; // maximum number of vertices we are allowed to create on the stack.
#endif


/**
\brief The maximum number of graphs that can be displayed at one time.  (0-5)
*/
static const PxU32 MAX_GRAPHS			(6);
/**
\brief The width of the graphs (x axis) when created automatically
*/
static const PxF32 GRAPH_WIDTH_DEFAULT	(+0.8f);
/**
\brief The height of the graphs (y axis) when created automatically
*/
static const PxF32 GRAPH_HEIGHT_DEFAULT	(+0.4f);

/**
\brief definition of the debugGraph descriptor used to create graphs
*/
struct debugGraphDesc
{
	/**
	\brief The number of PxReal data points to graph
	*/
	PxU32	mNumPoints;
	/**
	\brief Pointer to the PxReal data points to graph
	*/
	const PxF32*	mPoints;
	/**
	\brief optional cutoff line drawn horizontally on the graph.  It should be between 0 and mGraphMax.
	0.0f indicates not to draw the cutoff line.
	*/
	PxF32	mCutOffLevel;
	/**
	\brief The maximum value that the graph should be be able to display.
	Noramlly this is slightly larger than the greatest value in the mPoints array to make room for
	future samples that are higher then the current set.
	*/
	PxF32	mGraphMax;
	/**
	\brief The bottom left x coordinate of the graph.
	This is set automatically by the constructor that takes graphNum as an argument.
	NOTE: display coordinates range from -1.0f - + 1.0f
	*/
	PxF32	mGraphXPos;
	/**
	\brief bottom left y coordinate of the graph.
	This is set automatically by the constructor that takes graphNum as an argument.
	NOTE: display coordinates range from -1.0f - + 1.0f
	*/
	PxF32	mGraphYPos;
	/**
	\brief The width of the graph.
	This is set automatically by the constructor that takes graphNum as an argument.
	NOTE: display coordinates range from -1.0f - + 1.0f
	*/
	PxF32	mGraphWidth;
	/**
	\brief The height of the graph.
	This is set automatically by the constructor that takes graphNum as an argument.
	NOTE: display coordinates range from -1.0f - + 1.0f
	*/
	PxF32	mGraphHeight;
	/**
	\brief The color of the data.
	This is set automatically by the constructor that takes graphNum as an argument.
	NOTE: display coordinates range from -1.0f - + 1.0f
	This is set automatically by the constructor that takes graphNum as an argument.
	*/
	PxU32	mGraphColor;
	/**
	\brief The alternate color of the data if mColorSwitchIndex is set to a value that is a valid
	index to the mPoints array.  The points after mColorSwitchIndex are drawn in this color.
	This is set automatically by the constructor that takes graphNum as an argument.
	*/
	PxU32	mArrowColor;
	/**
	\brief A pointer to the label for the X axis.
	*/
	char*	mGraphXLabel;
	/**
	\brief A pointer to the label for the Y axis.
	*/
	char*	mGraphYLabel;
	/**
	\brief The (optional!) index in the data set at which to switch the color to the color specified by
	mArrorColor.  By default this is set to -1 indicating that no color switch should be performed.
	*/
	PxU32	mColorSwitchIndex;
};

/**
\brief State flags for debug renderable
 */
struct DebugRenderState
{
	enum Enum
	{
		ScreenSpace      = (1<<0),  //!< true if rendering in screenspace
		NoZbuffer        = (1<<1),  //!< true if zbuffering is disabled.
        SolidShaded      = (1<<2),  //!< true if rendering solid shaded.
        SolidWireShaded  = (1<<3),  //!< Render both as a solid shaded triangle and as a wireframe overlay.
        CounterClockwise = (1<<4),  //!< true if winding order is counter clockwise.
        CameraFacing     = (1<<5),  //!< True if text should be displayed camera facing
        InfiniteLifeSpan = (1<<6),  //!< True if the lifespan is infinite (overrides current display time value)
        CenterText       = (1<<7),  //!< True if the text should be centered.
		//DoubleSided      = (1<<8),  //!< Render the trinagle double sided *NOT IMPLEMENTED*
		//DoubleSidedWire  = (1<<9),  //!< Render the trinagle double sided with a wireframe outline. *NOT IMPLEMENTED*
	};
};

/**
\brief Enums for debug colors
 */
struct DebugColors
{
	enum Enum
	{
		Default = 0,
		PoseArrows,
		MeshStatic,
		MeshDynamic,
		Shape,
		Text0,
		Text1,
		Color0,
		Color1,
		Color2,
		Color3,
		Color4,
		Color5,
		Red,
		Green,
		Blue,
		DarkRed,
		DarkGreen,
		DarkBlue,
		LightRed,
		LightGreen,
		LightBlue,
		Purple,
		LightPurple,
		DarkPurple,
		Yellow,
		Orange,
		Gold,
		Emerald,
		White,
		Black,
		Gray,
		LightGray,
		DarkGray,
		NUM_COLORS
	};
};

/**
\brief Simple vertex with normal and color
 */
class RenderDebugSolidVertex
{
public:
	PxF32 mPos[3];
	PxF32 mNormal[3];
	PxU32 mColor;
};

/**
\brief simple vertex with color
 */
class RenderDebugVertex
{
public:

	PxF32 mPos[3];
	PxU32 mColor;
};

/**
\brief user provided rendering output methods
 */
class RenderDebugInterface
{
public:
  virtual void debugRenderLines(PxU32 lcount,const RenderDebugVertex *vertices,bool useZ,bool isScreenSpace) = 0;
  virtual void debugRenderTriangles(PxU32 tcount,const RenderDebugSolidVertex *vertices,bool useZ,bool isScreenSpace) = 0;

  virtual void debugRenderIndexedTriangleMesh(PxU32 triangleMeshId,const char *material,
	  PxU32 transformCount,
	  const PxF32 *localToWorld44,
	  const PxF32 *positions,
	  const PxF32 *normals,
	  const PxU32 *colors,
	  const PxF32 *tangents,
	  const PxF32 *binormals,
	  const PxF32 *texcoords0,
	  const PxF32 *texcoords1,
	  const PxF32 *texcoords2,
	  const PxF32 *texcoords3,
	  const PxU32  *indices,
	  PxU32 triCount,
	  PxU32 vertCount) = 0;
protected:
	virtual ~RenderDebugInterface(void)
	{

	}
};

/**
\brief class that draws debug rendering primitives
 */
class RenderDebug
{
public:
	virtual PxU32 getUpdateCount(void) const = 0;

    virtual bool render(PxF32 dtime,RenderDebugInterface *iface) = 0;

	virtual void debugGraph(PxU32 numPoints, PxF32 * points, PxF32 graphMax, PxF32 graphXPos, PxF32 graphYPos, PxF32 graphWidth, PxF32 graphHeight, PxU32 colorSwitchIndex = 0xFFFFFFFF) = 0;

    virtual void debugText(PxF32 x,PxF32 y,PxF32 z,const char *fmt,...) = 0;

	virtual void  reset(PxI32 blockIndex=-1) = 0; // -1 reset *everything*, 0 = reset everything except stuff inside blocks, > 0 reset a specific block of data.

	virtual void  drawGrid(bool zup=false,PxU32 gridSize=40) = 0; // draw a grid.

	virtual void  pushRenderState(void) = 0;

    virtual void  popRenderState(void) = 0;

	virtual void  setCurrentColor(PxU32 color=0xFFFFFF,PxU32 arrowColor=0xFF0000) = 0;
	virtual PxU32 getCurrentColor(void) const = 0;
	virtual PxU32 getCurrentArrowColor(void) const = 0;

	virtual void  setCurrentUserPointer(void *userPtr) = 0;

	virtual void  setCurrentUserId(PxI32 userId) = 0;

	virtual void * getCurrentUserPointer(void) const = 0;
	virtual PxI32 getCurrentUserId(void) = 0;

	virtual void  setCurrentDisplayTime(PxF32 displayTime=0.0001f) = 0;

	virtual PxF32 getRenderScale(void) = 0;

	virtual void  setRenderScale(PxF32 scale) = 0;

	virtual void  setCurrentState(PxU32 states=0) = 0;

	virtual void  addToCurrentState(DebugRenderState::Enum state) = 0; // OR this state flag into the current state.

	virtual void  removeFromCurrentState(DebugRenderState::Enum state) = 0; // Remove this bit flat from the current state

	virtual void  setCurrentTextScale(PxF32 textScale) = 0;

	virtual void  setCurrentArrowSize(PxF32 arrowSize) = 0;

	virtual PxU32 getCurrentState(void) const = 0;

	virtual void  setRenderState(PxU32 states=0,  // combination of render state flags
	                             PxU32 color=0xFFFFFF, // base color
                                 PxF32 displayTime=0.0001f, // duration of display items.
	                             PxU32 arrowColor=0xFF0000, // secondary color, usually used for arrow head
                                 PxF32 arrowSize=0.1f,
								 PxF32 renderScale=1.0f,
								 PxF32 textScale=1.0f) = 0;      // seconary size, usually used for arrow head size.


	virtual PxU32 getRenderState(PxU32 &color,PxF32 &displayTime,PxU32 &arrowColor,PxF32 &arrowSize,PxF32 &renderScale,PxF32 &textScale) const = 0;


	virtual void  endDrawGroup(void) = 0;

	virtual void  setDrawGroupVisible(PxI32 groupId,bool state) = 0;

	virtual void debugRect2d(PxF32 x1,PxF32 y1,PxF32 x2,PxF32 y2) = 0;

	virtual void  debugPolygon(PxU32 pcount,const physx::PxVec3 *points) = 0;

	virtual void  debugLine(const physx::PxVec3 &p1,const physx::PxVec3 &p2) = 0;

	virtual void  debugGradientLine(const physx::PxVec3 &p1,const physx::PxVec3 &p2,const PxU32 &c1,const PxU32 &c2) = 0;

	virtual void  debugOrientedLine(const physx::PxVec3 &p1,const physx::PxVec3 &p2,const PxMat44 &transform) = 0;
	virtual void  debugOrientedLine(const physx::PxVec3 &p1,const physx::PxVec3 &p2,const PxMat34Legacy &transform) = 0;

	virtual void  debugRay(const physx::PxVec3 &p1,const physx::PxVec3 &p2) = 0;

	virtual void  debugCylinder(const physx::PxVec3 &p1,const physx::PxVec3 &p2,PxF32 radius) = 0;

	virtual void  debugThickRay(const physx::PxVec3 &p1,const physx::PxVec3 &p2,PxF32 raySize=0.02f) = 0;

	virtual void  debugPlane(const physx::PxVec3 &normal,PxF32 dCoff,PxF32 radius1,PxF32 radius2) = 0;

	virtual void  debugTri(const physx::PxVec3 &p1,const physx::PxVec3 &p2,const physx::PxVec3 &p3) = 0;

	virtual void  debugTriNormals(const physx::PxVec3 &p1,const physx::PxVec3 &p2,const physx::PxVec3 &p3,const physx::PxVec3 &n1,const physx::PxVec3 &n2,const physx::PxVec3 &n3) = 0;

	virtual void  debugGradientTri(const physx::PxVec3 &p1,const physx::PxVec3 &p2,const physx::PxVec3 &p3,const PxU32 &c1,const PxU32 &c2,const PxU32 &c3) = 0;

	virtual void  debugGradientTriNormals(const physx::PxVec3 &p1,const physx::PxVec3 &p2,const physx::PxVec3 &p3,const physx::PxVec3 &n1,const physx::PxVec3 &n2,const physx::PxVec3 &n3,const PxU32 &c1,const PxU32 &c2,const PxU32 &c3) = 0;

	virtual void  debugBound(const physx::PxVec3 &bmin,const physx::PxVec3 &bmax) = 0;

	virtual void  debugBound(const physx::PxBounds3 &b) = 0;

	virtual void  debugOrientedBound(const physx::PxVec3 &sides,const PxMat44 &transform) = 0;
	virtual void  debugOrientedBound(const physx::PxVec3 &sides,const PxMat34Legacy &transform) = 0;

	virtual void  debugOrientedBound(const physx::PxVec3 &bmin,const physx::PxVec3 &bmax,const physx::PxVec3 &pos,const physx::PxQuat &quat) = 0; // the rotation as a quaternion

	virtual void  debugOrientedBound(const physx::PxVec3 &bmin,const physx::PxVec3 &bmax,const PxMat44 &xform) = 0; // the rotation as a quaternion
	virtual void  debugOrientedBound(const physx::PxVec3 &bmin,const physx::PxVec3 &bmax,const PxMat34Legacy &xform) = 0; // the rotation as a quaternion

	virtual void  debugSphere(const physx::PxVec3 &pos,PxF32 radius) = 0;

	virtual void  debugOrientedSphere(PxF32 radius,const PxMat44 &transform) = 0;
	virtual void  debugOrientedSphere(PxF32 radius,const PxMat34Legacy &transform) = 0;

	virtual void  debugOrientedCapsule(PxF32 radius,PxF32 height,PxU32 subdivision,const PxMat44 &transform) = 0;
	virtual void  debugOrientedCapsule(PxF32 radius,PxF32 height,PxU32 subdivision,const PxMat34Legacy &transform) = 0;

	virtual void  debugOrientedCylinder(PxF32 radius,PxF32 height,PxU32 subdivision,bool closeSides,const PxMat44 &transform) = 0;
	virtual void  debugOrientedCylinder(PxF32 radius,PxF32 height,PxU32 subdivision,bool closeSides,const PxMat34Legacy &transform) = 0;

	virtual void  debugPoint(const physx::PxVec3 &pos,PxF32 radius) = 0;

	virtual void  debugAxes(const PxMat44 &transform,PxF32 distance=0.1f,PxF32 brightness=1.0f) = 0;
	virtual void  debugAxes(const PxMat34Legacy &transform,PxF32 distance=0.1f,PxF32 brightness=1.0f) = 0;

    virtual void debugArc(const physx::PxVec3 &center,const physx::PxVec3 &p1,const physx::PxVec3 &p2,PxF32 arrowSize=0.1f,bool showRoot=false) = 0;

    virtual void debugThickArc(const physx::PxVec3 &center,const physx::PxVec3 &p1,const physx::PxVec3 &p2,PxF32 thickness=0.02f,bool showRoot=false) = 0;

    virtual void debugText(const physx::PxVec3 &pos,const char *fmt,...) = 0;

    virtual void debugOrientedText(const physx::PxVec3 &pos,const physx::PxQuat &rot,const char *fmt,...) = 0;

    virtual void debugOrientedText(const PxMat44 &xform,const char *fmt,...) = 0;
    virtual void debugOrientedText(const PxMat34Legacy &xform,const char *fmt,...) = 0;

	virtual void setViewMatrix(const PxF32 *view) = 0;

	virtual void setProjectionMatrix(const PxF32 *projection) = 0;

	virtual const PxF32* getViewProjectionMatrix(void) const = 0;
	virtual const PxF32 *getViewMatrix(void) const = 0;
	virtual const PxF32 *getProjectionMatrix(void) const = 0;

	virtual void  eulerToMatrix(const physx::PxVec3 &angles, physx::PxMat33Legacy &rot) = 0; // angles are in degrees
	virtual void  eulerToQuat(const physx::PxVec3 &angles, physx::PxQuat &q) = 0; // angles are in degrees.

	virtual PxI32 beginDrawGroup(const PxMat44 &pose) = 0;
	virtual PxI32 beginDrawGroup(const PxMat34Legacy &pose) = 0;

	virtual void  setDrawGroupPose(PxI32 blockId,const PxMat44 &pose) = 0;
	virtual void  setDrawGroupPose(PxI32 blockId,const PxMat34Legacy &pose) = 0;

	virtual void  debugDetailedSphere(const physx::PxVec3 &pos,PxF32 radius,PxU32 stepCount) = 0;

	virtual void  setPose(const PxMat44 &pose) = 0;
	virtual void  setPose(const PxMat34Legacy &pose) = 0;

	virtual const PxMat44 & getPose(void) const = 0;

	/* \brief Create an createDebugGraphDesc.  This is the manual way of setting up a graph.  Every parameter can
	and must be customized when using this constructor.
	*/
	virtual debugGraphDesc* createDebugGraphDesc(void) = 0;

	/**
	\brief Create an createDebugGraphDesc using the minimal amount of work.  This constructor provides for six custom
	graphs to be simultaneously drawn on the display at one time numbered 0 to 5.  The position, color, and size
	of the graphs are automatically set based on the graphNum argument.
	*/
	virtual debugGraphDesc* createDebugGraphDesc(PxU32 graphNum,PxU32 dataCount,const PxF32 *dataArray, PxF32 maxY, char* xLabel, char* yLabel) = 0;

	virtual void debugGraph(debugGraphDesc& graphDesc) = 0;

	/**
	\brief Return a debug color value by name.
	*/
	virtual PxU32 getDebugColor(DebugColors::Enum colorEnum) const = 0;

	/**
	\brief Return a debug color value by RGB inputs
	*/
	virtual PxU32 getDebugColor(float red, float green, float blue) const = 0;

	virtual void getDebugBounds(physx::PxBounds3 &b) const = 0;

	virtual PxF32 getFrameTime(void) const = 0;
	virtual void  setFrameTime(PxF32 ftime) = 0;

	virtual void renderIndexedTriangleMesh(PxU32 triangleMeshId,const char *material,
		PxU32 transformCount,
		const PxF32 *localToWorld44,
		const PxF32 *positions,
		const PxF32 *normals,
		const PxU32 *colors,
		const PxF32 *tangents,
		const PxF32 *binormals,
		const PxF32 *texcoords0,
		const PxF32 *texcoords1,
		const PxF32 *texcoords2,
		const PxF32 *texcoords3,
		const PxU32  *indices,
		PxU32 triCount,
		PxU32 vertCount) = 0;

	virtual	PxU32 getTriangleMeshId(void) = 0;

protected:

	virtual ~RenderDebug(void) { };

};


#pragma PX_POP_PACK

}; // end of namespace 
}; // end of namespace

#endif // __PX_RENDER_DEBUG_H__
