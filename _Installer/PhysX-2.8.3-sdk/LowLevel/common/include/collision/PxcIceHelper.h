#ifndef PXC_ICEHELPER_H
#define PXC_ICEHELPER_H

// Ice-includes
#include "Opcode.h"
#undef for

#include "PxcTransform.h"
#include "PxcMatrix34.h"

/*!
Helper functions for interaction with Opcode/ICE
*/

/*
Some notes:

ICE store matrices as m[Row][Col] with translation in fourth row. With multiplication order v' = v*M.
Pxc store matrices as m[Col][Row] with translation in fourth row. With multiplication order v' = M*v.
Thus, format is identical.
*/

PXD_INLINE void PxcToIce(IceMaths::Matrix4x4& iceMat44, const PxcMatrix34& pxcMat34)
{
	iceMat44.m[0][0] = pxcMat34[0][0];
	iceMat44.m[0][1] = pxcMat34[0][1];
	iceMat44.m[0][2] = pxcMat34[0][2];
	iceMat44.m[0][3] = 0.0f;
	iceMat44.m[1][0] = pxcMat34[1][0];
	iceMat44.m[1][1] = pxcMat34[1][1];
	iceMat44.m[1][2] = pxcMat34[1][2];
	iceMat44.m[1][3] = 0.0f;
	iceMat44.m[2][0] = pxcMat34[2][0];
	iceMat44.m[2][1] = pxcMat34[2][1];
	iceMat44.m[2][2] = pxcMat34[2][2];
	iceMat44.m[2][3] = 0.0f;
	iceMat44.m[3][0] = pxcMat34[3][0];
	iceMat44.m[3][1] = pxcMat34[3][1];
	iceMat44.m[3][2] = pxcMat34[3][2];
	iceMat44.m[3][3] = 1.0f;
}

PXD_INLINE void PxcToIce(IceMaths::Matrix4x4& iceMat44, const PxcTransform& transform)
{
	return PxcToIce(iceMat44, PxcMatrix34(transform));
}

#endif
