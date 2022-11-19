#include "NxPhysics.h"
#include "DrawObjects.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "common.h"

static float gPlaneData[]={
    -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

static float gCylinderData[]={
	1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,
	0.866025f,0.500000f,1.0f,0.866025f,0.500000f,1.0f,0.866025f,0.500000f,0.0f,0.866025f,0.500000f,0.0f,
	0.500000f,0.866025f,1.0f,0.500000f,0.866025f,1.0f,0.500000f,0.866025f,0.0f,0.500000f,0.866025f,0.0f,
	-0.0f,1.0f,1.0f,-0.0f,1.0f,1.0f,-0.0f,1.0f,0.0f,-0.0f,1.0f,0.0f,
	-0.500000f,0.866025f,1.0f,-0.500000f,0.866025f,1.0f,-0.500000f,0.866025f,0.0f,-0.500000f,0.866025f,0.0f,
	-0.866025f,0.500000f,1.0f,-0.866025f,0.500000f,1.0f,-0.866025f,0.500000f,0.0f,-0.866025f,0.500000f,0.0f,
	-1.0f,-0.0f,1.0f,-1.0f,-0.0f,1.0f,-1.0f,-0.0f,0.0f,-1.0f,-0.0f,0.0f,
	-0.866025f,-0.500000f,1.0f,-0.866025f,-0.500000f,1.0f,-0.866025f,-0.500000f,0.0f,-0.866025f,-0.500000f,0.0f,
	-0.500000f,-0.866025f,1.0f,-0.500000f,-0.866025f,1.0f,-0.500000f,-0.866025f,0.0f,-0.500000f,-0.866025f,0.0f,
	0.0f,-1.0f,1.0f,0.0f,-1.0f,1.0f,0.0f,-1.0f,0.0f,0.0f,-1.0f,0.0f,
	0.500000f,-0.866025f,1.0f,0.500000f,-0.866025f,1.0f,0.500000f,-0.866025f,0.0f,0.500000f,-0.866025f,0.0f,
	0.866026f,-0.500000f,1.0f,0.866026f,-0.500000f,1.0f,0.866026f,-0.500000f,0.0f,0.866026f,-0.500000f,0.0f,
	1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f
};

static float gCylinderDataCapsTop[]={
	0.866026f,-0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.500000f,-0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.500000f,-0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,-1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,-1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.500000f,-0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.500000f,-0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.866025f,-0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.866025f,-0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-1.000000f,-0.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-1.000000f,-0.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.866025f,0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.866025f,0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.500000f,0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.500000f,0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.500000f,0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.500000f,0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.866025f,0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.866025f,0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	1.000000f,0.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	1.000000f,0.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.866026f,-0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
};

static float gCylinderDataCapsBottom[]={
	1.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.866025f,0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.866025f,0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.500000f,0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.500000f,0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.000000f,1.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.000000f,1.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.500000f,0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.500000f,0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.866025f,0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.866025f,0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-1.000000f,-0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-1.000000f,-0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.866025f,-0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.866025f,-0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.500000f,-0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.500000f,-0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,-1.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,-1.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.500000f,-0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.500000f,-0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.866026f,-0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.866026f,-0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	1.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
};

static void RenderPlane()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), 6, gPlaneData);
    glNormalPointer(GL_FLOAT, 2*3*sizeof(float), 6, gPlaneData+3);

	glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

static void RenderBox(GLfloat size)
{
#if defined(_XBOX) | defined(__CELLOS_LV2__) | defined(__PPCGEKKO__)
    glutSolidCube(size);    // doesn't include texcoords
#else
    static GLfloat n[6][3] =
    {
        {-1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {1.0, 0.0, 0.0},
        {0.0, -1.0, 0.0},
        {0.0, 0.0, 1.0},
        {0.0, 0.0, -1.0}
    };
    static GLint faces[6][4] =
    {
        {0, 1, 2, 3},
        {3, 2, 6, 7},
        {7, 6, 5, 4},
        {4, 5, 1, 0},
        {5, 6, 2, 1},
        {7, 4, 0, 3}
    };
    GLfloat v[8][3];
    GLint i;

    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

    for (i = 5; i >= 0; i--) {
        glBegin(GL_QUADS);
        glNormal3fv(&n[i][0]);
        glTexCoord2f(0.0f, 0.0f); glVertex3fv(&v[faces[i][0]][0]);
        glTexCoord2f(1.0f, 0.0f); glVertex3fv(&v[faces[i][1]][0]);
        glTexCoord2f(1.0f, 1.0f); glVertex3fv(&v[faces[i][2]][0]);
        glTexCoord2f(0.0f, 1.0f); glVertex3fv(&v[faces[i][3]][0]);
        glEnd();
    }
#endif
}

static void RenderSphere()
{
#if 1
	glutSolidSphere(1.0f, 12, 12);  // doesn't include texcoords
#else
    GLUquadricObj *quadObj = gluNewQuadric();
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    gluQuadricTexture(quadObj, GL_TRUE);
    gluSphere(quadObj, 1.0f, 12, 12);
    gluDeleteQuadric(quadObj);
#endif
}

static void RenderCylinder()
{
#ifdef __PPCGEKKO__
	glutSolidCylinder(12);
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gCylinderData);
    glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gCylinderData+3);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 13*2);

    glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gCylinderDataCapsTop);
    glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gCylinderDataCapsTop+3);
	glDrawArrays(GL_TRIANGLES, 0, 36);

    glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gCylinderDataCapsBottom);
    glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gCylinderDataCapsBottom+3);
	glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
#endif
}

void SetupGLMatrix(const NxVec3& pos, const NxMat33& orient)
{
	float glmat[16];	//4x4 column major matrix for OpenGL.
	orient.getColumnMajorStride4(&(glmat[0]));
	pos.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));
}

void DrawLine(const NxVec3& p0, const NxVec3& p1, const NxVec3& color, float lineWidth)
{
	glDisable(GL_LIGHTING);
	glLineWidth(lineWidth);
	glColor4f(color.x, color.y, color.z, 1.0f);
#ifdef __PPCGEKKO__
	glBegin(GL_LINES,2);
	glVertex3d(p0.x, p0.y, p0.z);
	glVertex3d(p1.x, p1.y, p1.z);
	glEnd();
#else
	NxVec3 av3LineEndpoints[] = {p0, p1};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), &av3LineEndpoints[0].x);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
}

void DrawTriangle(const NxVec3& p0, const NxVec3& p1, const NxVec3& p2, const NxVec3& color)
{
	glDisable(GL_LIGHTING);
	glColor4f(color.x, color.y, color.z, 1.0f);
#ifdef __PPCGEKKO__
	glBegin(GL_TRIANGLES,3);
	glVertex3d(p0.x, p0.y, p0.z);
	glVertex3d(p1.x, p1.y, p1.z);
	glVertex3d(p2.x, p2.y, p2.z);
	glEnd();
#else
	NxVec3 av3LineEndpoints[] = {p0, p1, p2};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), &av3LineEndpoints[0].x);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
}

void DrawCircle(NxU32 nbSegments, const NxMat34& matrix, const NxVec3& color, const NxF32 radius, const bool semicircle)
{
	NxF32 step = NxTwoPiF32/NxF32(nbSegments);
	NxU32 segs = nbSegments;
	if(semicircle)
	{
		segs /= 2;
	}

	for(NxU32 i=0;i<segs;i++)
	{
		NxU32 j=i+1;
		if(j==nbSegments)	j=0;

		NxF32 angle0 = NxF32(i)*step;
		NxF32 angle1 = NxF32(j)*step;

		NxVec3 p0,p1;
		matrix.multiply(NxVec3(radius * sinf(angle0), radius * cosf(angle0), 0.0f), p0);
		matrix.multiply(NxVec3(radius * sinf(angle1), radius * cosf(angle1), 0.0f), p1);

		DrawLine(p0, p1, color);
	}
}


void DrawEllipse(NxU32 nbSegments, const NxMat34& matrix, const NxVec3& color, const NxF32 radius1, const NxF32 radius2, const bool semicircle)
{
	NxF32 step = NxTwoPiF32/NxF32(nbSegments);
	NxU32 segs = nbSegments;
	if(semicircle)
	{
		segs /= 2;
	}

	for(NxU32 i=0;i<segs;i++)
	{
		NxU32 j=i+1;
		if(j==nbSegments)	j=0;

		NxF32 angle0 = NxF32(i)*step;
		NxF32 angle1 = NxF32(j)*step;

		NxVec3 p0,p1;
		matrix.multiply(NxVec3(radius1 * sinf(angle0), radius2 * cosf(angle0), 0.0f), p0);
		matrix.multiply(NxVec3(radius1 * sinf(angle1), radius2 * cosf(angle1), 0.0f), p1);

		DrawLine(p0, p1, color);
	}
}

void DrawWirePlane(NxShape* plane, const NxVec3& color)
{
}

void DrawPlane(NxShape* plane)
{
	NxPlaneShape* planeShape = plane->isPlane();
	NxPlane p = planeShape->getPlane();
	NxVec3 n=p.normal;
	n.normalize();
	NxVec3 t1;
	NxVec3 t2;
	static const NxReal cos45 = 0.7071067811865475244008443621048490;
	if (fabs(n.z) > cos45) 
	{
		NxReal a = n.y*n.y + n.z*n.z;
		NxReal k = NxReal(1.0)/NxMath::sqrt(a);
		t1.set(0,-n.z*k,n.y*k);
		t2.set(a*k,-n.x*t1.z,n.x*t1.y);
	}
	else 
	{
		NxReal a = n.x*n.x + n.y*n.y;
		NxReal k = NxReal(1.0)/NxMath::sqrt(a);
		t1.set(-n.y*k,n.x*k,0);
		t2.set(-n.z*t1.y,n.z*t1.x,a*k);
	}
	NxMat34 pose;
	pose.M.setColumn(0, t2);
	pose.M.setColumn(1, n);
	pose.M.setColumn(2, t1);
	// We can't use this cause someone decided it was a good idea to support two plane representations
	// pose.t=p.pointInPlane();
	pose.t = n * p.d;
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
	SetupGLMatrix(pose.t, pose.M);
	glTranslatef(0,-0.1f,0);
	glScalef(1024,1,1024);
	RenderPlane();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void DrawWireBox(const NxBox& obb, const NxVec3& color, float lineWidth)
{
	// Compute obb vertices
	NxVec3 pp[8];
	NxComputeBoxPoints(obb, pp);
		
	// Draw all lines
	const NxU32* Indices = NxGetBoxEdges();
	for(NxU32 i=0;i<12;i++)
	{
		NxU32 VRef0 = *Indices++;
		NxU32 VRef1 = *Indices++;
		DrawLine(pp[VRef0], pp[VRef1], color, lineWidth);
	}
}

void DrawWireBox(NxShape* box, const NxVec3& color, float lineWidth)
{
	NxBox obb;
	box->isBox()->getWorldOBB(obb);

	DrawWireBox(obb, color, lineWidth);
}

void DrawBox(NxShape* box, const NxVec3& color)
{
	NxMat34 pose = box->getGlobalPose();

	glPushMatrix();
	SetupGLMatrix(pose.t, pose.M);
	NxVec3 boxDim = box->isBox()->getDimensions();
	glScalef(boxDim.x, boxDim.y, boxDim.z);
	RenderBox(2.0);
	glPopMatrix();
}

void DrawWireSphere(NxShape* sphere, const NxVec3& color)
{
	NxMat34 pose = sphere->getGlobalPose();

	glPushMatrix();
	NxReal r = sphere->isSphere()->getRadius();

	NxVec3 c0;	pose.M.getColumn(0, c0);
	NxVec3 c1;	pose.M.getColumn(1, c1);
	NxVec3 c2;	pose.M.getColumn(2, c2);
	DrawCircle(20, pose, color, r);

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	DrawCircle(20, pose, color, r);

	pose.M.setColumn(0, c2);
	pose.M.setColumn(1, c0);
	pose.M.setColumn(2, c1);
	DrawCircle(20, pose, color, r);

	glPopMatrix();
}

void DrawSphere(NxShape* sphere, const NxVec3& color)
{
	NxMat34 pose = sphere->getGlobalPose();

	glPushMatrix();
	SetupGLMatrix(pose.t, pose.M);
	NxReal r = sphere->isSphere()->getRadius();
	glScalef(r,r,r);
	RenderSphere();
	glPopMatrix();
}

void DrawWireCapsule(NxShape* capsule, const NxVec3& color)
{
	NxMat34 pose = capsule->getGlobalPose();

	NxReal r = capsule->isCapsule()->getRadius();
	NxReal h = capsule->isCapsule()->getHeight();

	NxSegment SG;
	pose.M.getColumn(1, SG.p1);
	SG.p1 *= 0.5f*h;
	SG.p0 = -SG.p1;
	SG.p0 += pose.t;
	SG.p1 += pose.t;

	NxVec3 c0;	pose.M.getColumn(0, c0);
	NxVec3 c1;	pose.M.getColumn(1, c1);
	NxVec3 c2;	pose.M.getColumn(2, c2);
	DrawLine(SG.p0 + c0*r, SG.p1 + c0*r, color);
	DrawLine(SG.p0 - c0*r, SG.p1 - c0*r, color);
	DrawLine(SG.p0 + c2*r, SG.p1 + c2*r, color);
	DrawLine(SG.p0 - c2*r, SG.p1 - c2*r, color);

	pose.M.setColumn(0, -c1);
	pose.M.setColumn(1, -c0);
	pose.M.setColumn(2, c2);
	pose.t = SG.p0;
	DrawCircle(20, pose, color, r, true);	//halfcircle -- flipped

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, -c0);
	pose.M.setColumn(2, c2);
	pose.t = SG.p1;
	DrawCircle(20, pose, color, r, true);

	pose.M.setColumn(0, -c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	pose.t = SG.p0;
	DrawCircle(20, pose, color, r, true);//halfcircle -- good

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	pose.t = SG.p1;
	DrawCircle(20, pose, color, r, true);

	pose.M.setColumn(0, c2);
	pose.M.setColumn(1, c0);
	pose.M.setColumn(2, c1);
	pose.t = SG.p0;
	DrawCircle(20, pose, color, r);	//full circle
	pose.t = SG.p1;
	DrawCircle(20, pose, color, r);
}

static void computeBasis(const NxVec3& dir, NxVec3& right, NxVec3& up)
{
	// Derive two remaining vectors
	if(fabsf(dir.y)>0.9999f)	right = NxVec3(1.0f, 0.0f, 0.0f);
	else						right = (NxVec3(0.0f, 1.0f, 0.0f) ^ dir);
	right.normalize();
	up = dir ^ right;
}

void DrawWireCapsule(const NxCapsule& capsule, const NxVec3& color)
{
	NxReal r = capsule.radius;
	NxVec3 dir = capsule.p0 - capsule.p1;
	NxReal h = dir.magnitude();
	NxMat34 pose;
	pose.t = (capsule.p0 + capsule.p1)*0.5f;

	if(h!=0.0f)
	{
		dir/=h;
		NxVec3 right, up;
		computeBasis(dir, right, up);
		pose.M.setColumn(0, right);
		pose.M.setColumn(1, dir);
		pose.M.setColumn(2, up);
	}
	else
	{
		pose.M.id();
	}

//	NxMat34 pose = capsule->getGlobalPose();
//	const NxReal & r = capsule->isCapsule()->getRadius();
//	const NxReal & h = capsule->isCapsule()->getHeight();



	NxSegment SG;
	pose.M.getColumn(1, SG.p1);
	SG.p1 *= 0.5f*h;
	SG.p0 = -SG.p1;
	SG.p0 += pose.t;
	SG.p1 += pose.t;

	NxVec3 c0;	pose.M.getColumn(0, c0);
	NxVec3 c1;	pose.M.getColumn(1, c1);
	NxVec3 c2;	pose.M.getColumn(2, c2);
	DrawLine(SG.p0 + c0*r, SG.p1 + c0*r, color);
	DrawLine(SG.p0 - c0*r, SG.p1 - c0*r, color);
	DrawLine(SG.p0 + c2*r, SG.p1 + c2*r, color);
	DrawLine(SG.p0 - c2*r, SG.p1 - c2*r, color);

	pose.M.setColumn(0, -c1);
	pose.M.setColumn(1, -c0);
	pose.M.setColumn(2, c2);
	pose.t = SG.p0;
	DrawCircle(20, pose, color, r, true);	//halfcircle -- flipped

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, -c0);
	pose.M.setColumn(2, c2);
	pose.t = SG.p1;
	DrawCircle(20, pose, color, r, true);

	pose.M.setColumn(0, -c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	pose.t = SG.p0;
	DrawCircle(20, pose, color, r, true);//halfcircle -- good

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	pose.t = SG.p1;
	DrawCircle(20, pose, color, r, true);

	pose.M.setColumn(0, c2);
	pose.M.setColumn(1, c0);
	pose.M.setColumn(2, c1);
	pose.t = SG.p0;
	DrawCircle(20, pose, color, r);	//full circle
	pose.t = SG.p1;
	DrawCircle(20, pose, color, r);
}

void DrawCapsule(NxShape* capsule, const NxVec3& color)
{
	NxMat34 pose = capsule->getGlobalPose();

	const NxReal & r = capsule->isCapsule()->getRadius();
	const NxReal & h = capsule->isCapsule()->getHeight();

	glPushMatrix();
	SetupGLMatrix(pose.t, pose.M);

	glPushMatrix();
	glTranslatef(0.0f, h*0.5f, 0.0f);
	glScalef(r,r,r);
	RenderSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f,-h*0.5f, 0.0f);
	glScalef(r,r,r);
	RenderSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f,h*0.5f, 0.0f);
	glScalef(r,h,r);
	glRotatef(90.0f,1.0f,0.0f,0.0f);
	RenderCylinder();
	glPopMatrix();

	glPopMatrix();
}

void DrawCapsule(const NxVec3& color, NxF32 r, NxF32 h)
{
	glColor4f(color.x, color.y, color.z, 1.0f);

	glPushMatrix();
	glTranslatef(0.0f, h*0.5f, 0.0f);
	glScalef(r,r,r);
	RenderSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f,-h*0.5f, 0.0f);
	glScalef(r,r,r);
	RenderSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f,h*0.5f, 0.0f);
	glScalef(r,h,r);
	glRotatef(90.0f,1.0f,0.0f,0.0f);
	RenderCylinder();
	glPopMatrix();
}

typedef NxVec3 Point;
typedef struct _Triangle { NxU32 p0; NxU32 p1; NxU32 p2; } Triangle;

void DrawWireConvex(NxShape* mesh, const NxVec3& color)
{
    if(mesh->userData == NULL) return;

	NxMat34 pose = mesh->getGlobalPose();

	NxConvexMeshDesc meshDesc = *((NxConvexMeshDesc*)(mesh->userData));
//	mesh->isConvexMesh()->getConvexMesh().saveToDesc(meshDesc);

	NxU32 nbVerts = meshDesc.numVertices;
	NxU32 nbTriangles = meshDesc.numTriangles;

	Point* points = (Point *)meshDesc.points;
	Triangle* triangles = (Triangle *)meshDesc.triangles;

	glPushMatrix();

	float glmat[16];	//4x4 column major matrix for OpenGL.
	pose.M.getColumnMajorStride4(&(glmat[0]));
	pose.t.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));

	while(nbTriangles--)
	{
		DrawLine(points[triangles->p0], points[triangles->p1], color);
		DrawLine(points[triangles->p1], points[triangles->p2], color);
		DrawLine(points[triangles->p2], points[triangles->p0], color);
		triangles++;
	}

	glPopMatrix();
}

void DrawTriangleList(int iTriangleCount, Triangle *pTriangles, Point *pPoints)
{
	static int iBufferSize=0;
	static float *pfVertexBuffer=NULL;
	static float *pfNormalBuffer=NULL;

	if(iBufferSize < iTriangleCount*3)
	{
		iBufferSize=3*iTriangleCount;

		if( pfVertexBuffer )
		delete[] pfVertexBuffer;
		pfVertexBuffer = new float[iBufferSize*3];

		if( pfNormalBuffer )
		delete[] pfNormalBuffer;
		pfNormalBuffer = new float[iBufferSize*3];
	}

	float *pfDestinationVertex=pfVertexBuffer;
	float *pfDestinationNormal=pfNormalBuffer;

	for(int iTriangle=0; iTriangle<iTriangleCount; iTriangle++)
	{
		*pfDestinationVertex++=pPoints[pTriangles->p0].x;
		*pfDestinationVertex++=pPoints[pTriangles->p0].y;
		*pfDestinationVertex++=pPoints[pTriangles->p0].z;
		*pfDestinationVertex++=pPoints[pTriangles->p1].x;
		*pfDestinationVertex++=pPoints[pTriangles->p1].y;
		*pfDestinationVertex++=pPoints[pTriangles->p1].z;
		*pfDestinationVertex++=pPoints[pTriangles->p2].x;
		*pfDestinationVertex++=pPoints[pTriangles->p2].y;
		*pfDestinationVertex++=pPoints[pTriangles->p2].z;

		NxVec3 edge1 = pPoints[pTriangles->p1] - pPoints[pTriangles->p0];
		NxVec3 edge2 = pPoints[pTriangles->p2] - pPoints[pTriangles->p0];
		NxVec3 normal = edge1.cross(edge2);
		normal.normalize();

		*pfDestinationNormal++=normal.x;
		*pfDestinationNormal++=normal.y;
		*pfDestinationNormal++=normal.z;
		*pfDestinationNormal++=normal.x;
		*pfDestinationNormal++=normal.y;
		*pfDestinationNormal++=normal.z;
		*pfDestinationNormal++=normal.x;
		*pfDestinationNormal++=normal.y;
		*pfDestinationNormal++=normal.z;

		pTriangles++;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, iTriangleCount*3, pfVertexBuffer);
	glNormalPointer(GL_FLOAT, 0, iTriangleCount*3, pfNormalBuffer);

	glDrawArrays(GL_TRIANGLES, 0, 3*iTriangleCount);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}

void DrawConvex(NxShape* mesh, const NxVec3& color)
{
    NxConvexMeshDesc meshDesc;

	// SRM : Please note that I looked into a crash issue posed by
	// one of our customers, and this code (i.e. reinterpreting the
	// NxShape's userData as an NxConvexMeshDesc * was crashing because
	// in the SampleRaycastCar demo, it sets that pointer equal to the
	// NxWheel because that is used in NxVehicle::handleContactPair().  Thus
	// in order to allow this code not to crash on the PS3, we should
	// simply force the shape to save its description here.
	mesh->isConvexMesh()->getConvexMesh().saveToDesc(meshDesc);
	/**
	if(mesh->userData != NULL)
	{
		meshDesc = *((NxConvexMeshDesc*)(mesh->userData));
	} else {
		mesh->isConvexMesh()->getConvexMesh().saveToDesc(meshDesc);
	}
	**/

	NxMat34 pose = mesh->getGlobalPose();

	NxU32 nbVerts = meshDesc.numVertices;
	NxU32 nbTriangles = meshDesc.numTriangles;

	Point* points = (Point *)meshDesc.points;
	Triangle* triangles = (Triangle *)meshDesc.triangles;

	glPushMatrix();

	float glmat[16];	//4x4 column major matrix for OpenGL.
	pose.M.getColumnMajorStride4(&(glmat[0]));
	pose.t.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));

	DrawTriangleList(nbTriangles, triangles, points);
	
	glPopMatrix();
}

void DrawWireMesh(NxShape* mesh, const NxVec3& color)
{
    if(mesh->userData == NULL) return;

	NxMat34 pose = mesh->getGlobalPose();

	NxTriangleMeshDesc meshDesc = *((NxTriangleMeshDesc*)(mesh->userData));
//	mesh->isTriangleMesh()->getTriangleMesh().saveToDesc(meshDesc);

	NxU32 nbVerts = meshDesc.numVertices;
	NxU32 nbTriangles = meshDesc.numTriangles;

	Point* points = (Point *)meshDesc.points;
	Triangle* triangles = (Triangle *)meshDesc.triangles;

	glPushMatrix();

	float glmat[16];	//4x4 column major matrix for OpenGL.
	pose.M.getColumnMajorStride4(&(glmat[0]));
	pose.t.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));

	while(nbTriangles--)
	{
		DrawLine(points[triangles->p0], points[triangles->p1], color);
		DrawLine(points[triangles->p1], points[triangles->p2], color);
		DrawLine(points[triangles->p2], points[triangles->p0], color);
		triangles++;
	}

	glPopMatrix();
}

void DrawMesh(NxShape* mesh, const NxVec3& color)
{
    if(mesh->userData == NULL) return;

	NxMat34 pose = mesh->getGlobalPose();

	NxTriangleMeshDesc meshDesc = *((NxTriangleMeshDesc*)(mesh->userData));
//	mesh->isTriangleMesh()->getTriangleMesh().saveToDesc(meshDesc);

	NxU32 nbVerts = meshDesc.numVertices;
	NxU32 nbTriangles = meshDesc.numTriangles;

	Point* points = (Point *)meshDesc.points;
	Triangle* triangles = (Triangle *)meshDesc.triangles;

	glPushMatrix();

	float glmat[16];	//4x4 column major matrix for OpenGL.
	pose.M.getColumnMajorStride4(&(glmat[0]));
	pose.t.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));

	if(meshDesc.heightFieldVerticalAxis != NX_NOT_HEIGHTFIELD) 
	{
	    glDisable(GL_LIGHT0);
	    glEnable(GL_LIGHT1);
	}

	DrawTriangleList(nbTriangles, triangles, points);

	if(meshDesc.heightFieldVerticalAxis != NX_NOT_HEIGHTFIELD) 
	{
	    glDisable(GL_LIGHT1);
	    glEnable(GL_LIGHT0);
	}
	
	glPopMatrix();
}

void DrawWheelShape(NxShape* wheel)
{
	if(wheel->is(NX_SHAPE_WHEEL) != NULL)
	{
		NxWheelShape* wheelShape = (NxWheelShape*)wheel;
		glPushMatrix();

		float glmat[16];
		wheel->getGlobalPose().getColumnMajor44(glmat);

		NxWheelShapeDesc wheelShapeDesc;

		float r = wheelShape->getRadius();
		float a = wheelShape->getSteerAngle();

		glMultMatrixf(&(glmat[0]));
		glTranslatef(-r/2,0,0);
		glRotatef(90,0,1,0);
		glRotatef(NxMath::radToDeg(a),0,1,0);

		glScalef(r, r, r);
		RenderCylinder();
		glPopMatrix();
	}
}

void DrawArrow(const NxVec3& posA, const NxVec3& posB, const NxVec3& color)
{
	NxVec3 vec = posB - posA;
	NxVec3 t0, t1, t2;
	NxNormalToTangents(vec, t1, t2);

	t0 = posB - posA;
	t0.normalize();

	NxVec3 lobe1  = posB - t0*0.15 + t1 * 0.15;
	NxVec3 lobe2  = posB - t0*0.15 - t1 * 0.15;
	NxVec3 lobe3  = posB - t0*0.15 + t2 * 0.15;
	NxVec3 lobe4  = posB - t0*0.15 - t2 * 0.15;

	NxVec3 v3ArrowShape[] = {
		NxVec3(posA), NxVec3(posB),
		NxVec3(posB), NxVec3(lobe1),
		NxVec3(posB), NxVec3(lobe2),
		NxVec3(posB), NxVec3(lobe3),
		NxVec3(posB), NxVec3(lobe4),
	};

	glDisable(GL_LIGHTING);
	glLineWidth(3.0f);
	glColor4f(color.x,color.y,color.z,1.0f);
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), 10, &v3ArrowShape[0].x);

	glDrawArrays(GL_LINES, 0, sizeof(v3ArrowShape)/sizeof(NxVec3));
	glDisableClientState(GL_VERTEX_ARRAY);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

void DrawContactPoint(const NxVec3& pos, const NxReal radius, const NxVec3& color)
{
	NxMat34 pose;
	pose.t = pos;

	NxVec3 c0;	pose.M.getColumn(0, c0);
	NxVec3 c1;	pose.M.getColumn(1, c1);
	NxVec3 c2;	pose.M.getColumn(2, c2);
	DrawCircle(20, pose, color, radius);

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	DrawCircle(20, pose, color, radius);

	pose.M.setColumn(0, c2);
	pose.M.setColumn(1, c0);
	pose.M.setColumn(2, c1);
	DrawCircle(20, pose, color, radius);
}

void DrawWireShape(NxShape *shape, const NxVec3& color)
{
    switch(shape->getType())
    {
		case NX_SHAPE_PLANE:
			DrawWirePlane(shape, color);
		break;
		case NX_SHAPE_BOX:
			DrawWireBox(shape, color);
		break;
		case NX_SHAPE_SPHERE:
			DrawWireSphere(shape, color);
		break;
		case NX_SHAPE_CAPSULE:
			DrawWireCapsule(shape, color);
		break;
		case NX_SHAPE_CONVEX:
			DrawWireConvex(shape, color);
		break;		
		case NX_SHAPE_MESH:
			DrawWireMesh(shape, color);
		break;
		default:
		break;
	}
}

void DrawShape(NxShape* shape, const NxVec3& color)
{
    switch(shape->getType())
    {
		case NX_SHAPE_PLANE:
			DrawPlane(shape);
		break;
		case NX_SHAPE_BOX:
			DrawBox(shape, color);
		break;
		case NX_SHAPE_SPHERE:
			DrawSphere(shape, color);
		break;
		case NX_SHAPE_CAPSULE:
			DrawCapsule(shape, color);
		break;
		case NX_SHAPE_CONVEX:
			DrawConvex(shape, color);
		break;
		case NX_SHAPE_MESH:
			DrawMesh(shape, color);
		break;
		case NX_SHAPE_WHEEL:
			DrawWheelShape(shape);
		break;
		default:
		break;
	}
}

void DrawActor(NxActor* actor, NxActor* gSelectedActor)
{
	NxShape*const* shapes = actor->getShapes();
	NxU32 nShapes = actor->getNbShapes();
	if (actor == gSelectedActor) 
	{
		while (nShapes--)
		{
			DrawWireShape(shapes[nShapes], NxVec3(1,1,1));
		}
	}

	NxVec3 defaultColor = NxVec3(0.65f, 0.67f, 0.63f);
	NxVec3 dynamicActiveColor = NxVec3(0.82f, 0.77f, 0.73f);
	NxVec3 dynamicSleepingColor = NxVec3(0.6f, 0.6f, 0.6f);
	NxVec3 kinematicColor = NxVec3(0.73f, 0.77f, 0.82f);

	nShapes = actor->getNbShapes();
	while (nShapes--)
	{
		NxVec3 color = defaultColor;
		if (actor->isDynamic())
		{
			if (actor->readBodyFlag(NX_BF_KINEMATIC))
				color = kinematicColor;
			else
				if (actor->isSleeping())
					color = dynamicSleepingColor;
				else
					color = dynamicActiveColor;
		}
		DrawShape(shapes[nShapes], color);
	}
}

void DrawWireActor(NxActor* actor)
{
	NxShape*const* shapes = actor->getShapes();
	NxU32 nShapes = actor->getNbShapes();
	nShapes = actor->getNbShapes();
	while (nShapes--)
	{
		DrawWireShape(shapes[nShapes], NxVec3(1,1,1));
	}
}

static void DrawActorShadow(NxActor* actor, const float* ShadowMat)
{
	glPushMatrix();
	glMultMatrixf(ShadowMat);

	glDisable(GL_LIGHTING);
	glColor4f(0.05f, 0.1f, 0.15f, 1.0f);
	
	NxShape*const* shapes = actor->getShapes();
	NxU32 nShapes = actor->getNbShapes();
	while (nShapes--)
	{
		//Ignore the shapes below d=0 plane
		if(shapes[nShapes]->getGlobalPosition().y < 0)
			continue;
		switch(shapes[nShapes]->getType())
		{
		    case NX_SHAPE_BOX:
			    DrawBox(shapes[nShapes]);
			break;
		    case NX_SHAPE_SPHERE:
			    DrawSphere(shapes[nShapes]);
			break;
		    case NX_SHAPE_CAPSULE:
			    DrawCapsule(shapes[nShapes]);
			break;
		    case NX_SHAPE_CONVEX:
			    DrawConvex(shapes[nShapes]);
			break;	
			default:
			break;
		}
	}	
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}


void DrawActorShadow(NxActor* actor)
{
	const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
	DrawActorShadow(actor, ShadowMat);
}

void DrawActorShadow2(NxActor* actor)
{
    const static float ShadowMat[]={ 1,0,0,0, 1,0,-0.2,0, 0,0,1,0, 0,0,0,1 };
	DrawActorShadow(actor, ShadowMat);
}

void DrawActorShadowZUp(NxActor* actor)
{
    const static float ShadowMat[]={ 1,0,0,0, 0,1,0,0, 0,0,0,0, 0,0,0,1 };
	DrawActorShadow(actor, ShadowMat);
}

void DrawCloth(NxCloth *cloth, bool shadows)
{
	NxMeshData meshData = cloth->getMeshData();

	NxU32 numVertices = *meshData.numVerticesPtr;
	NxU32 numTriangles = *meshData.numIndicesPtr / 3;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, numVertices, meshData.verticesPosBegin);
	glNormalPointer(GL_FLOAT, 0, numVertices, meshData.verticesNormalBegin);

#ifdef __CELLOS_LV2__	
	glDrawRangeElements(GL_TRIANGLES, 0, numVertices-1, 3*numTriangles, GL_UNSIGNED_INT, meshData.indicesBegin);
#else
	glDrawElements(GL_TRIANGLES, 3*numTriangles, GL_UNSIGNED_INT, meshData.indicesBegin);
#endif

	if (shadows) {
		const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
		glPushMatrix();
		glMultMatrixf(ShadowMat);
		glDisable(GL_LIGHTING);
		glColor4f(0.05f, 0.1f, 0.15f,1.0f);

#ifdef __CELLOS_LV2__	
		glDrawRangeElements(GL_TRIANGLES, 0, numVertices-1, 3*numTriangles, GL_UNSIGNED_INT, meshData.indicesBegin);
#else
		glDrawElements(GL_TRIANGLES, 3*numTriangles, GL_UNSIGNED_INT, meshData.indicesBegin);
#endif
		
		glColor4f(1.0f, 1.0f, 1.0f,1.0f);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}


void DrawFluid(NxFluid* fluid)
{
#ifndef NX_DISABLE_FLUIDS
	NxParticleData particles = fluid->getParticlesWriteData();
	if (!particles.numParticlesPtr)
		return;

	NxU32 numParticles = *particles.numParticlesPtr;
	if (numParticles == 0)
		return;

#if !defined(__CELLOS_LV2__)&&!defined(__PPCGEKKO__)
	glPushAttrib(GL_ALL_ATTRIB_BITS);
#endif

	glPointSize(5.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	
	glVertexPointer(3, GL_FLOAT, particles.bufferPosByteStride, numParticles, particles.bufferPos);

	glDrawArrays(GL_POINTS, 0, numParticles);

	glDisableClientState(GL_VERTEX_ARRAY);

#if !defined(__CELLOS_LV2__)&&!defined(__PPCGEKKO__)
	glPopAttrib();
#endif
#endif // NX_DISABLE_FLUIDS
}

#ifdef WIN32
#include <windows.h>
/*
typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
int vsyncState = 1;
*/

//function pointer typdefs
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);
 
//declare functions
PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;


#endif

void setVSync(bool on)
{
#ifdef WIN32
	if (wglSwapIntervalEXT == NULL)
	{
		const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
		if (strstr(extensions, "WGL_EXT_swap_control") != 0)
		{
			 wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
		}
	}

	if (wglSwapIntervalEXT != NULL)
	{
		wglSwapIntervalEXT(on ? 1.f : 0.f);
	}
#endif
}

bool getVSync()
{
#ifdef WIN32
	if (wglGetSwapIntervalEXT == NULL)
	{
		const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
		if (strstr(extensions, "WGL_EXT_swap_control") != 0)
		{
			 wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)wglGetProcAddress( "wglGetSwapIntervalEXT" );
		}
	}
	if (wglGetSwapIntervalEXT != NULL)
	{
		return (wglGetSwapIntervalEXT() > 0);
	}
#endif
	return true;
}

void toggleVSync()
{
	setVSync(!getVSync());
}
