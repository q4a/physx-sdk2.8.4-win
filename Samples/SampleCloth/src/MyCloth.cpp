/*----------------------------------------------------------------------------*\
|
|						    NVIDIA PhysX Technology
|
|							    www.nvidia.com
|
\*----------------------------------------------------------------------------*/

//Test Cloth Implementation

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX		//suppress windows' global min,max macros.
#include <windows.h>	//needed for gl.h
#endif

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include "MyCloth.h"
#include "Stream.h"
#include "wavefront.h"
#include "BmpLoader.h"
#include "NxCooking.h"
#include "cooking.h"
#include "common.h"

#define TEAR_MEMORY_FACTOR 3

// -----------------------------------------------------------------------
MyCloth::MyCloth(NxScene *scene, NxClothDesc &desc, const char *objFileName, NxReal scale, NxVec3* offset, const char *texFilename)
  : mVertexRenderBuffer(NULL), mScene(NULL), mCloth(NULL), mClothMesh(NULL),
    mIndexRenderBuffer(NULL), mTempTexCoords(NULL), mNumTempTexCoords(0), mTexId(0), mTeared(false)
{
	mInitDone = false;
	NxClothMeshDesc meshDesc;
	generateObjMeshDesc(meshDesc, objFileName, scale, offset, texFilename != NULL);
	// todo: handle failure
	init(scene, desc, meshDesc);
	if (texFilename)
		createTexture(texFilename);
}


// -----------------------------------------------------------------------
MyCloth::MyCloth(NxScene *scene, NxClothDesc &desc, NxReal w, NxReal h, NxReal d, const char *texFilename, bool tearLines)
  : mVertexRenderBuffer(NULL), mScene(NULL), mCloth(NULL), mClothMesh(NULL),
    mIndexRenderBuffer(NULL), mTempTexCoords(NULL), mNumTempTexCoords(0), mTexId(0), mTeared(false)
{
	mInitDone = false;
	NxClothMeshDesc meshDesc;
	generateRegularMeshDesc(meshDesc, w, h, d, texFilename != NULL, tearLines);
	init(scene, desc, meshDesc);
	if (texFilename) 
		createTexture(texFilename);
}

// -----------------------------------------------------------------------
void MyCloth::init(NxScene *scene, NxClothDesc &desc, NxClothMeshDesc &meshDesc)
{
	mScene = scene;

	// if we want tearing we must tell the cooker
	// this way it will generate some space for particles that will be generated during tearing
	if (desc.flags & NX_CLF_TEARABLE)
		meshDesc.flags |= NX_CLOTH_MESH_TEARABLE;

	// Enable Debug rendering for this cloth
	desc.flags |= NX_CLF_VISUALIZATION;

	cookMesh(meshDesc);
	releaseMeshDescBuffers(meshDesc);

	allocateReceiveBuffers(meshDesc.numVertices, meshDesc.numTriangles);

	desc.clothMesh = mClothMesh;
	desc.meshData = mReceiveBuffers;
	mCloth = scene->createCloth(desc);
	mInitDone = true;
}

// -----------------------------------------------------------------------
MyCloth::~MyCloth()
{
	if (mInitDone) {
		if (mCloth) mScene->releaseCloth(*mCloth);
		if (mClothMesh) mScene->getPhysicsSDK().releaseClothMesh(*mClothMesh);
		releaseReceiveBuffers();

		// temp texcoords are only used temporary and should be deallocated way earlier
		assert(mTempTexCoords == NULL);

		if (mTexId) {
			glDeleteTextures(1, &mTexId);
			mTexId = 0;
		}

		// delete the rendering buffers
		free (mVertexRenderBuffer);
		free (mIndexRenderBuffer);
	}
}

// -----------------------------------------------------------------------
bool MyCloth::generateObjMeshDesc(NxClothMeshDesc &desc, const char *filename, NxReal scale, NxVec3* offset, bool textured)
{
	WavefrontObj wo;
	wo.loadObj(filename, textured);
	if (wo.mVertexCount == 0) return false;

	NxVec3 myOffset(0.f);
	if (offset != NULL)
		myOffset = *offset;

	desc.numVertices				= wo.mVertexCount;
	desc.numTriangles				= wo.mTriCount;
	desc.pointStrideBytes			= sizeof(NxVec3);
	desc.triangleStrideBytes		= 3*sizeof(NxU32);
	desc.vertexMassStrideBytes		= sizeof(NxReal);
	desc.vertexFlagStrideBytes		= sizeof(NxU32);
	desc.points						= (NxVec3*)malloc(sizeof(NxVec3)*desc.numVertices);
	desc.triangles					= (NxU32*)malloc(sizeof(NxU32)*desc.numTriangles*3);
	desc.vertexMasses				= 0;
	desc.vertexFlags				= 0;
	desc.flags						= NX_CLOTH_MESH_WELD_VERTICES;
	desc.weldingDistance			= 0.0001f;

	mMaxVertices = TEAR_MEMORY_FACTOR * wo.mVertexCount;
	mMaxIndices  = 3 * wo.mTriCount;

	// copy positions and indices
	NxVec3 *vSrc = (NxVec3*)wo.mVertices;
	NxVec3 *vDest = (NxVec3*)desc.points;
	for (int i = 0; i < wo.mVertexCount; i++, vDest++, vSrc++) 
		*vDest = (*vSrc)*scale + myOffset;
	memcpy((NxU32*)desc.triangles, wo.mIndices, sizeof(NxU32)*desc.numTriangles*3);

	if (textured)
	{
		mNumTempTexCoords = desc.numVertices;
		mTempTexCoords = (GLfloat *)malloc(sizeof(GLfloat) * mNumTempTexCoords * 2);
		memcpy((NxU32*)mTempTexCoords, wo.mTexCoords, sizeof(GLfloat)*mNumTempTexCoords*2);

		mIndexRenderBuffer = (NxU32*)malloc(sizeof(NxU32) * mMaxIndices);
		memset(mIndexRenderBuffer, 0, sizeof(NxU32) * mMaxIndices);
		for (NxU32 i = 0; i < desc.numTriangles; i++)
		{
			assert((desc.flags & NX_CF_16_BIT_INDICES) == 0);
			NxU32* tri = (NxU32*)(((char*)desc.triangles) + (desc.triangleStrideBytes * i));
			mIndexRenderBuffer[3*i+0] = tri[0];
			mIndexRenderBuffer[3*i+1] = tri[1];
			mIndexRenderBuffer[3*i+2] = tri[2];
		}
	}
	else
	{
		mNumTempTexCoords = 0;
		mTempTexCoords = NULL;
	}

	return true;
}

// -----------------------------------------------------------------------
void MyCloth::generateRegularMeshDesc(NxClothMeshDesc &desc, NxReal w, NxReal h, NxReal d, bool texCoords, bool tearLines)
{
	int numX = (int)(w / d) + 1;
	int numY = (int)(h / d) + 1;

	desc.numVertices				= (numX+1) * (numY+1);
	desc.numTriangles				= numX*numY*2;
	desc.pointStrideBytes			= sizeof(NxVec3);
	desc.triangleStrideBytes		= 3*sizeof(NxU32);
	desc.vertexMassStrideBytes		= sizeof(NxReal);
	desc.vertexFlagStrideBytes		= sizeof(NxU32);
	desc.points						= (NxVec3*)malloc(sizeof(NxVec3)*desc.numVertices);
	desc.triangles					= (NxU32*)malloc(sizeof(NxU32)*desc.numTriangles*3);
	desc.vertexMasses				= 0;
	desc.vertexFlags				= 0;
	desc.flags						= 0;

	mMaxVertices = TEAR_MEMORY_FACTOR * desc.numVertices;
	mMaxIndices  = 3 * desc.numTriangles;

	int i,j;
	NxVec3 *p = (NxVec3*)desc.points;
	for (i = 0; i <= numY; i++) {
		for (j = 0; j <= numX; j++) {
			p->set(d*j, 0.0f, d*i); 
			p++;
		}
	}

	if (texCoords) {
		mTempTexCoords = (GLfloat *)malloc(sizeof(GLfloat)*2*TEAR_MEMORY_FACTOR*desc.numVertices);
		GLfloat *f = mTempTexCoords;
		GLfloat dx = 1.0f; if (numX > 0) dx /= numX;
		GLfloat dy = 1.0f; if (numY > 0) dy /= numY;
		for (i = 0; i <= numY; i++) {
			for (j = 0; j <= numX; j++) {
				*f++ = j*dx;
				*f++ = i*dy;
			}
		}
		mNumTempTexCoords = desc.numVertices;
	}
	else
	{
		mNumTempTexCoords = 0;
		mTempTexCoords = NULL;
	}

	NxU32 *id = (NxU32*)desc.triangles;
	for (i = 0; i < numY; i++) {
		for (j = 0; j < numX; j++) {
			NxU32 i0 = i * (numX+1) + j;
			NxU32 i1 = i0 + 1;
			NxU32 i2 = i0 + (numX+1);
			NxU32 i3 = i2 + 1;
			if ((j+i)%2) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}

	// generate tear lines if necessary
	if(tearLines)
		generateTearLines(desc, numX + 1, numY + 1);
}

// -----------------------------------------------------------------------
void MyCloth::generateTearLines(NxClothMeshDesc& desc, NxU32 w, NxU32 h)
{
	// allocate flag buffer
	if(desc.vertexFlags == 0)
		desc.vertexFlags = malloc(sizeof(NxU32)*desc.numVertices);

	// create tear lines
	NxU32* flags = (NxU32*)desc.vertexFlags;
	NxU32 y;
	for(y = 0; y < h; y++)
	{
		NxU32 x;
		for(x = 0; x < w; x++)
		{
			if(((x + y) % 16 == 0) || ((x - y + 16) % 16 == 0))
				flags[y * w + x] = NX_CLOTH_VERTEX_TEARABLE;
			else
				flags[y * w + x] = 0;
		}
	}
}

// -----------------------------------------------------------------------
void MyCloth::releaseMeshDescBuffers(const NxClothMeshDesc& desc)
{
	NxVec3* p = (NxVec3*)desc.points;
	NxU32* t = (NxU32*)desc.triangles;
	NxReal* m = (NxReal*)desc.vertexMasses;
	NxU32* f = (NxU32*)desc.vertexFlags;
	free(p);
	free(t);
	free(m);
	free(f);
}

// -----------------------------------------------------------------------
bool MyCloth::cookMesh(NxClothMeshDesc& desc)
{
	// Store correct number to detect tearing mesh in time
	mLastNumVertices = desc.numVertices;

	// we cook the mesh on the fly through a memory stream
	// we could also use a file stream and pre-cook the mesh
	MemoryWriteBuffer wb;
	assert(desc.isValid());
	bool success = CookClothMesh(desc, wb);

	if (!success) 
		return false;

	MemoryReadBuffer rb(wb.data);
	mClothMesh = mScene->getPhysicsSDK().createClothMesh(rb);
	return true;
}

// -----------------------------------------------------------------------
void MyCloth::allocateReceiveBuffers(int numVertices, int numTriangles)
{
	// here we setup the buffers through which the SDK returns the dynamic cloth data
	// we reserve more memory for vertices than the initial mesh takes
	// because tearing creates new vertices
	// the SDK only tears cloth as long as there is room in these buffers

	//NxU32 maxVertices = TEAR_MEMORY_FACTOR * numVertices;
	//NxU32 maxIndices = 3*numTriangles;

	if (mVertexRenderBuffer == NULL)
	{
		// Allocate Render Buffer for Vertices if it hasn't been done before
		mVertexRenderBuffer = (RenderBufferVertexElement*)malloc(sizeof(RenderBufferVertexElement) * mMaxVertices);
		memset(mVertexRenderBuffer, 0, sizeof(RenderBufferVertexElement) * mMaxVertices);
	}

	if (mIndexRenderBuffer == NULL)
	{
		// Allocate Render Buffer for Indices if it hasn't been done before
		mIndexRenderBuffer = (NxU32*)malloc(sizeof(NxU32) * mMaxIndices);
		memset(mIndexRenderBuffer, 0, sizeof(NxU32) * mMaxIndices);
	}

	mReceiveBuffers.verticesPosBegin         = &(mVertexRenderBuffer[0].position.x);
	mReceiveBuffers.verticesNormalBegin      = &(mVertexRenderBuffer[0].normal.x);
	mReceiveBuffers.verticesPosByteStride    = sizeof(RenderBufferVertexElement);
	mReceiveBuffers.verticesNormalByteStride = sizeof(RenderBufferVertexElement);
	mReceiveBuffers.maxVertices              = mMaxVertices;
	mReceiveBuffers.numVerticesPtr           = &mNumVertices;

	// the number of triangles is constant, even if the cloth is torn
	NxU32 maxIndices = 3*numTriangles;
	mReceiveBuffers.indicesBegin             = mIndexRenderBuffer;
	mReceiveBuffers.indicesByteStride        = sizeof(NxU32);
	mReceiveBuffers.maxIndices               = maxIndices;
	mReceiveBuffers.numIndicesPtr            = &mNumIndices;

	if (mNumTempTexCoords > 0)
	{
		// Copy Tex Coords from temp buffers to graphics buffer
		assert(mNumTempTexCoords == numVertices);

		for (NxU32 i = 0; i < mNumTempTexCoords; i++)
		{
			mVertexRenderBuffer[i].texCoord[0] = mTempTexCoords[2*i+0];
			mVertexRenderBuffer[i].texCoord[1] = mTempTexCoords[2*i+1];
		}

		// Get rid of temp buffer
		mNumTempTexCoords = 0;
		free (mTempTexCoords);
		mTempTexCoords = NULL;
	}

	// the parent index information would be needed if we used textured cloth
	mReceiveBuffers.parentIndicesBegin       = (NxU32*)malloc(sizeof(NxU32)*mMaxVertices);
	mReceiveBuffers.parentIndicesByteStride  = sizeof(NxU32);
	mReceiveBuffers.maxParentIndices         = mMaxVertices;
	mReceiveBuffers.numParentIndicesPtr      = &mNumParentIndices;

	mReceiveBuffers.dirtyBufferFlagsPtr = &mMeshDirtyFlags;

	// init the buffers in case we want to draw the mesh 
	// before the SDK as filled in the correct values
	mMeshDirtyFlags = 0;
	mNumVertices = 0;
	mNumIndices = 0;
	mNumParentIndices = 0;
}

// -----------------------------------------------------------------------
void MyCloth::releaseReceiveBuffers()
{
	// Parent Indices is always allocated
	free (mReceiveBuffers.parentIndicesBegin);

	mReceiveBuffers.setToDefault();
}

// -----------------------------------------------------------------------
void MyCloth::draw(bool shadows)
{
	static NxU32 numVertices = mNumVertices;
	NxU32 numElements = mNumIndices;
	numVertices = mNumVertices;

	// Disable pressure if tearing occurs
	if (mTeared && (mCloth->getFlags() & NX_CLF_PRESSURE))
	{
		// Disable Pressure
		mCloth->setFlags(mCloth->getFlags() & ~NX_CLF_PRESSURE);
		mCloth->setPressure(0);

		// Reduce tearing factor
		NxReal oldTearing = mCloth->getTearFactor();
		oldTearing = (oldTearing - 1) / 3 + 1;
		mCloth->setTearFactor(oldTearing);

		// Reduce bending stiffness
		if (mCloth->getBendingStiffness() > 0.9f)
			mCloth->setBendingStiffness(0.2f);

		// Apply explosion in the middle of the cloth
		NxBounds3 bounds;
		mCloth->getWorldBounds(bounds);
		NxVec3 center;
		bounds.getCenter(center);
		NxReal radius = bounds.min.distance(bounds.max);
		mCloth->addForceAtPos(center, 7 * NxMath::pow(radius,3), radius, NX_IMPULSE);
		printf("Pressure disabled\n");
	}

	if (mTexId > 0)
	{
		updateTextureCoordinates();
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(RenderBufferVertexElement), numVertices, &(mVertexRenderBuffer[0].position.x));
	glNormalPointer(GL_FLOAT, sizeof(RenderBufferVertexElement), numVertices, &(mVertexRenderBuffer[0].normal.x));

	if (mTexId) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, sizeof(RenderBufferVertexElement), numVertices, &(mVertexRenderBuffer[0].texCoord[0]));

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mTexId);
		glColor4f(1.0f, 1.0f, 1.0f,1.0f);
	}

#ifdef __CELLOS_LV2__	
	glDrawRangeElements(GL_TRIANGLES, 0, numVertices-1, numElements, GL_UNSIGNED_INT, mIndexRenderBuffer);
#else
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, mIndexRenderBuffer);
#endif

	if (mTexId) {
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	if (shadows) {
		const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
		glPushMatrix();
		glMultMatrixf(ShadowMat);
		glDisable(GL_LIGHTING);
		glColor4f(0.05f, 0.1f, 0.15f,1.0f);

#ifdef __CELLOS_LV2__	
		glDrawRangeElements(GL_TRIANGLES, 0, numVertices-1, numElements, GL_UNSIGNED_INT, mIndexRenderBuffer);
#else
		glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, mIndexRenderBuffer);
#endif
		
		glColor4f(1.0f, 1.0f, 1.0f,1.0f);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}


// -----------------------------------------------------------------------
bool MyCloth::createTexture(const char *filename)
{
	BmpLoader bl;
	if (!bl.loadBmp(filename)) return false;

	glGenTextures(1, &mTexId);
	if (!mTexId) return false;
	glBindTexture(GL_TEXTURE_2D, mTexId);
#ifndef __PPCGEKKO__
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		bl.mWidth, bl.mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bl.mRGB);

	return true;
}

// -----------------------------------------------------------------------
void MyCloth::updateTextureCoordinates()
{
	NxU32 numVertices = *mReceiveBuffers.numVerticesPtr;
	NxU32 *parent = (NxU32 *)mReceiveBuffers.parentIndicesBegin + mLastNumVertices;
	for (NxU32 i = mLastNumVertices; i < numVertices; i++, parent++) {
		mVertexRenderBuffer[i].texCoord[0] = mVertexRenderBuffer[*parent].texCoord[0];
		mVertexRenderBuffer[i].texCoord[1] = mVertexRenderBuffer[*parent].texCoord[1];
	}

	mTeared |= mLastNumVertices != numVertices;
	mLastNumVertices = numVertices;
}
