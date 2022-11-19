#include "OrthographicDrawing.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <NxFoundation.h>

#include "GLFontRenderer.h"

#include "common.h"

void OrthographicDrawing::setOrthographicProjection(float w, float h)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	GLFontRenderer::setScreenResolution((int)w, (int)h);

	mWidth = w;
	mHeight = h;
}

void OrthographicDrawing::resetPerspectiveProjection()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void OrthographicDrawing::drawCircle(float posX, float posY, float radius, float fromDegree, float toDegree, float stepDegree, int bigMarks, int smallMarks)
{

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	float fromRad = NxMath::degToRad(fromDegree);
	float toRad = NxMath::degToRad(toDegree);
	float stepRad = NxMath::degToRad(stepDegree);
	int lineLen = int(((toRad-fromRad)/stepRad)*2);

	static int vecBufferSize = 0;
	static NxVec3* pVecBuffer = NULL;

	if(vecBufferSize < lineLen)
	{
		delete[] pVecBuffer;
		pVecBuffer = new NxVec3[lineLen];
		vecBufferSize = lineLen;
	}

	float angle = fromRad;
	for(int i = 0; i < lineLen/2; angle += stepRad, i++)
	{
		float Sin, Cos;
		NxMath::sinCos(angle, Sin, Cos);
		pVecBuffer[i*2+0].set(posX + Cos*radius, posY - Sin*radius, 0);
		NxMath::sinCos(angle+stepRad, Sin, Cos);
		pVecBuffer[i*2+1].set(posX + Cos*radius, posY - Sin*radius, 0);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
#ifdef __PPCGEKKO__
	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), vecBufferSize, pVecBuffer);
#else	
	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), pVecBuffer);
#endif
	glDrawArrays(GL_LINES, 0, lineLen);
	glDisableClientState(GL_VERTEX_ARRAY);

	if (bigMarks > 0) {
		stepRad = (toRad - fromRad) / (float)bigMarks;
		float bigRadius = radius * 1.2f;
		float smallRadius = radius * 0.8f;
		float toRad2 = toRad + 0.01f;

		int lineLen = int(((toRad2-fromRad)/stepRad)+1);

		if(vecBufferSize < lineLen*2)
		{
			delete[] pVecBuffer;
			pVecBuffer = new NxVec3[lineLen*2];
			vecBufferSize = lineLen*2;
		}

		float angle = fromRad;
		for(int i = 0; i < lineLen; angle += stepRad, i++)
		{
			float Sin, Cos;
			NxMath::sinCos(angle, Sin, Cos);
			pVecBuffer[i*2+0].set(posX + Cos*bigRadius, posY - Sin*bigRadius, 0);
			pVecBuffer[i*2+1].set(posX + Cos*smallRadius, posY - Sin*smallRadius, 0);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
#ifdef __PPCGEKKO__
	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), vecBufferSize, pVecBuffer);
#else
		glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), pVecBuffer);
#endif
		glDrawArrays(GL_LINES, 0, lineLen*2);
		glDisableClientState(GL_VERTEX_ARRAY);	

	}

	if (smallMarks > 0) {
		stepRad = (toRad - fromRad) / (float)smallMarks;
		float bigRadius = radius * 1.1f;
		float smallRadius = radius * 0.9f;
		float toRad2 = toRad + 0.01f;
		int lineLen = int(((toRad2-fromRad)/stepRad)+1);

		if(vecBufferSize < lineLen*2)
		{
			delete[] pVecBuffer;
			pVecBuffer = new NxVec3[lineLen*2];
			vecBufferSize = lineLen*2;
		}
		
		float angle = fromRad;
		for(int i = 0; i < lineLen; angle += stepRad, i++)		
		{
			float Sin, Cos;
			NxMath::sinCos(angle, Sin, Cos);
			pVecBuffer[i*2+0].set(posX + Cos*bigRadius, posY - Sin*bigRadius, 0);
			pVecBuffer[i*2+1].set(posX + Cos*smallRadius, posY - Sin*smallRadius, 0);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
#ifdef __PPCGEKKO__
	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), vecBufferSize, pVecBuffer);
#else
		glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), pVecBuffer);
#endif
		glDrawArrays(GL_LINES, 0, lineLen*2);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	
}

void OrthographicDrawing::drawLine(float x1, float y1, float x2, float y2)
{
#ifdef __PPCGEKKO__
	glBegin(GL_LINES, 2);
	glVertex3d(x1, y1, 0);
	glVertex3d(x2, y2, 0);
	glEnd();
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	float line[2][3] = {{x1, y1, 0},{x2, y2, 0}};
	glVertexPointer(3, GL_FLOAT, 0, line);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

void OrthographicDrawing::drawText(int xpos, int ypos, const char* pText)
{

	float x = (float)xpos/(float)glutGet(GLUT_WINDOW_WIDTH);
	float y = 1-(float)ypos/(float)glutGet(GLUT_WINDOW_HEIGHT);

	int len = (int)strlen(pText);
	len = (len < 256)?len:255;
	int start = 0;
	char textBuffer[256];
	for(int i=0;i<len;i++)
	{
		if(pText[i] == '\n' || i == len-1)
		{
			int offset = i;
			if(i == len-1) offset= i+1;
			memcpy(textBuffer, pText+start, offset-start);
			textBuffer[offset-start]=0;
#ifdef __PPCGEKKO__
			GLFontRenderer::print(x, y, 0.025f*512.0f/(float)glutGet(GLUT_WINDOW_HEIGHT), textBuffer, false, 11, true);
#else
			GLFontRenderer::print(x, y, 0.025f*512.0f/(float)glutGet(GLUT_WINDOW_HEIGHT), textBuffer, false, 11, false);
#endif
			y -= 0.035f;
			start = offset+1;
		}
	}
}
