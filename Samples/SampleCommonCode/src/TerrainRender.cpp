
#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#undef random
#include "NxPhysics.h"
#include "Terrain.h"
#include "TerrainRender.h"
#include "common.h"

void renderTerrain(const TerrainData& terrain, bool addWireframe)
{
	float* pVertList = new float[terrain.nbFaces*3*3];
	float* pNormList = new float[terrain.nbFaces*3*3];
	float* pColorList = new float[terrain.nbFaces*4*3];

	const NxU32* faces = terrain.faces;
	const NxVec3* colors = terrain.colors;
	const NxVec3* normals = terrain.normals;
	const NxVec3* verts = terrain.verts;

	int vertIndex = 0;
	int normIndex = 0;
	int colorIndex = 0;
	for(int i=0;i<(int)terrain.nbFaces;i++)
	{
		for(int j=0;j<3;j++)
		{
		pVertList[vertIndex++] = verts[faces[i*3+j]].x;
		pVertList[vertIndex++] = verts[faces[i*3+j]].y;
		pVertList[vertIndex++] = verts[faces[i*3+j]].z;

		pNormList[normIndex++] = normals[faces[i*3+j]].x;
		pNormList[normIndex++] = normals[faces[i*3+j]].y;
		pNormList[normIndex++] = normals[faces[i*3+j]].z;

		pColorList[colorIndex++] = colors[faces[i*3+j]].x;
		pColorList[colorIndex++] = colors[faces[i*3+j]].y;
		pColorList[colorIndex++] = colors[faces[i*3+j]].z;
		pColorList[colorIndex++] = 1.0f;
		}
	}

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3,GL_FLOAT, 0, terrain.nbFaces*3, pVertList);
	glNormalPointer(GL_FLOAT, 0, terrain.nbFaces*3,pNormList);
    glColorPointer(4,GL_FLOAT, 0, terrain.nbFaces*3,pColorList);

    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLES, 0, terrain.nbFaces*3);

	if(addWireframe)
	{
	
#ifndef __PPCGEKKO__
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		
#endif
		glVertexPointer(3,GL_FLOAT, 0, terrain.nbFaces*3, pVertList);
		glNormalPointer(GL_FLOAT, 0, terrain.nbFaces*3, pNormList);
		
	
		glDisableClientState(GL_COLOR_ARRAY);

		glLineWidth(2.0f);

		glColor4f(0.2f, 0.2f, 0.6f, 1.0f);
#ifdef __PPCGEKKO__
		glDrawArrays(GL_LINES, 0, terrain.nbFaces*3);
#else		
		glDrawArrays(GL_TRIANGLES, 0, terrain.nbFaces*3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
#endif
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

	delete[] pVertList;
	delete[] pNormList;
	delete[] pColorList;
}

void renderTerrainTriangles(const TerrainData& terrain, NxU32 nbTriangles, const NxU32* indices)
{
	float* pVertList = new float[nbTriangles*3*3];
	float* pNormList = new float[nbTriangles*3*3];

	const NxU32* faces = terrain.faces;
	const NxVec3* normals = terrain.normals;
	const NxVec3* verts = terrain.verts;

	int vertIndex = 0;
	int normIndex = 0;
	for(int i=0;i<(int)nbTriangles;i++)
	{
		NxU32 index = *indices++;

		for(int j=0;j<3;j++)
		{
		pVertList[vertIndex++] = verts[faces[index*3+j]].x;
		pVertList[vertIndex++] = verts[faces[index*3+j]].y;
#ifdef __PPCGEKKO__
		pVertList[vertIndex++] = verts[faces[index*3+j]].z+0.2;
#else		
		pVertList[vertIndex++] = verts[faces[index*3+j]].z;
#endif	

		pNormList[normIndex++] = normals[faces[index*3+j]].x;
		pNormList[normIndex++] = normals[faces[index*3+j]].y;
		pNormList[normIndex++] = normals[faces[index*3+j]].z;
		}
	}

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3,GL_FLOAT, 0, nbTriangles*3, pVertList);
	glNormalPointer(GL_FLOAT, 0, nbTriangles*3, pNormList);
	
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, nbTriangles*3);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

	delete[] pVertList;
	delete[] pNormList;
}
