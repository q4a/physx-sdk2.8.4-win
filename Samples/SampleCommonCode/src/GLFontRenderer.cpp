#include <math.h>
#include <stdio.h>
#include <string.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "GLFontData.h"
#include "GLFontRenderer.h"

#include "common.h"

bool GLFontRenderer::m_isInit=false;
unsigned int GLFontRenderer::m_textureObject=0;
int GLFontRenderer::m_screenWidth=640;
int GLFontRenderer::m_screenHeight=480;
float GLFontRenderer::m_color[4]={1.0f, 1.0f, 1.0f, 1.0f};

bool GLFontRenderer::init()
{
	glGenTextures(1, (GLuint*)&m_textureObject);
	if(m_textureObject == 0) return false;

	glBindTexture(GL_TEXTURE_2D, m_textureObject);
#ifndef __PPCGEKKO__
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif

	// expand to rgba
	unsigned char* pNewSource = new unsigned char[OGL_FONT_TEXTURE_WIDTH*OGL_FONT_TEXTURE_HEIGHT*4];
	for(int i=0;i<OGL_FONT_TEXTURE_WIDTH*OGL_FONT_TEXTURE_HEIGHT;i++)
	{
		pNewSource[i*4+0]=255;
		pNewSource[i*4+1]=255;
		pNewSource[i*4+2]=255;
		pNewSource[i*4+3]=OGLFontData[i];
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, OGL_FONT_TEXTURE_WIDTH, OGL_FONT_TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pNewSource);
	delete[] pNewSource;

	return true;
}

void GLFontRenderer::print(float x, float y, float fontSize, const char* pString, bool forceMonoSpace, int monoSpaceWidth, bool doOrthoProj)
{
	x = x*m_screenWidth;
#ifdef __PPCGEKKO__	
	y = (1-y)*m_screenHeight;
#else
	y = y*m_screenHeight;
#endif
	fontSize = fontSize*m_screenHeight;

	if(!m_isInit)
	{
		m_isInit = init();
	}

	unsigned int num = (unsigned int)strlen(pString);
	if(m_isInit && num > 0)
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_textureObject);


		if(doOrthoProj)
		{
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
#ifdef __PPCGEKKO__
			glOrtho(0, m_screenWidth, m_screenHeight, 0,  -1, 1);
			gluLookAt(0, 0, 0, 0, 0, -1.0f, 0.0f, 1.0f, 0.0f); //sorry need identity view matrix
#else
			glOrtho(0, m_screenWidth, 0, m_screenHeight, -1, 1);
#endif
		}
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glEnable(GL_BLEND);

		glColor4f(m_color[0], m_color[1], m_color[2], m_color[3]);

		const float glyphHeightUV = ((float)OGL_FONT_CHARS_PER_COL)/OGL_FONT_TEXTURE_HEIGHT*2-0.01f;

		float translate = 0.0f;

#ifdef __PPCGEKKO__
		static float* pVertList = NULL;
		static float* pTextureCoordList = NULL;
		
		static int pVertNum = 0;
		
		if( pVertNum < num )
		{
			if(!pVertList)
				delete pVertList;
			if(!pTextureCoordList)
				delete pTextureCoordList;
			
			pVertNum = num;
			pVertList = new float[pVertNum*3*6];
			pTextureCoordList = new float[pVertNum*2*6];
		}
#else
		float* pVertList = new float[num*3*6];
    	float* pTextureCoordList = new float[num*2*6];
#endif
		int vertIndex = 0;
    	int textureCoordIndex = 0;

		float translateDown = 0.0f;
		unsigned int count = 0;

		for(unsigned int i=0;i<num; i++)
		{
			const float glyphWidthUV = ((float)OGL_FONT_CHARS_PER_ROW)/OGL_FONT_TEXTURE_WIDTH;

			if (pString[i] == '\n') {
#ifdef __PPCGEKKO__
				translateDown+=0.005f*m_screenHeight+fontSize;
#else
				translateDown-=0.005f*m_screenHeight+fontSize;
#endif
				translate = 0.0f;
				continue;
			}

			int c = pString[i]-OGL_FONT_CHAR_BASE;
			if (c < OGL_FONT_CHARS_PER_ROW*OGL_FONT_CHARS_PER_COL) {

				count++;

				float glyphWidth = (float)GLFontGlyphWidth[c];
				if(forceMonoSpace){
					glyphWidth = (float)monoSpaceWidth;
				}
				
				glyphWidth = glyphWidth*(fontSize/(((float)OGL_FONT_TEXTURE_WIDTH)/OGL_FONT_CHARS_PER_ROW))-0.01f;

				float cxUV = float((c)%OGL_FONT_CHARS_PER_ROW)/OGL_FONT_CHARS_PER_ROW+0.008f;
				float cyUV = float((c)/OGL_FONT_CHARS_PER_ROW)/OGL_FONT_CHARS_PER_COL+0.008f;

				pTextureCoordList[textureCoordIndex++] = cxUV;
#ifdef __PPCGEKKO__
				pTextureCoordList[textureCoordIndex++] = cyUV;
#else
				pTextureCoordList[textureCoordIndex++] = cyUV+glyphHeightUV;
#endif		

				pVertList[vertIndex++] = x+0+translate;
				pVertList[vertIndex++] = y+0+translateDown;
				pVertList[vertIndex++] = 0;

				pTextureCoordList[textureCoordIndex++] = cxUV+glyphWidthUV;
				
#ifdef __PPCGEKKO__
				pTextureCoordList[textureCoordIndex++] = cyUV+glyphHeightUV;
#else
				pTextureCoordList[textureCoordIndex++] = cyUV;
#endif


				pVertList[vertIndex++] = x+fontSize+translate;
				pVertList[vertIndex++] = y+fontSize+translateDown;
				pVertList[vertIndex++] = 0;

				pTextureCoordList[textureCoordIndex++] = cxUV;
#ifdef __PPCGEKKO__
				pTextureCoordList[textureCoordIndex++] = cyUV+glyphHeightUV;
#else
				pTextureCoordList[textureCoordIndex++] = cyUV;
#endif

				pVertList[vertIndex++] = x+0+translate;
				pVertList[vertIndex++] = y+fontSize+translateDown;
				pVertList[vertIndex++] = 0;

				pTextureCoordList[textureCoordIndex++] = cxUV;
#ifdef __PPCGEKKO__
				pTextureCoordList[textureCoordIndex++] = cyUV;
#else
				pTextureCoordList[textureCoordIndex++] = cyUV+glyphHeightUV;
#endif	
				pVertList[vertIndex++] = x+0+translate;
				pVertList[vertIndex++] = y+0+translateDown;
				pVertList[vertIndex++] = 0;

				pTextureCoordList[textureCoordIndex++] = cxUV+glyphWidthUV;
#ifdef __PPCGEKKO__
				pTextureCoordList[textureCoordIndex++] = cyUV;
#else
				pTextureCoordList[textureCoordIndex++] = cyUV+glyphHeightUV;
#endif
				
				pVertList[vertIndex++] = x+fontSize+translate;
				pVertList[vertIndex++] = y+0+translateDown;
				pVertList[vertIndex++] = 0;

				pTextureCoordList[textureCoordIndex++] = cxUV+glyphWidthUV;
#ifdef __PPCGEKKO__
				pTextureCoordList[textureCoordIndex++] = cyUV+glyphHeightUV;
#else
				pTextureCoordList[textureCoordIndex++] = cyUV;
#endif
				
				pVertList[vertIndex++] = x+fontSize+translate;
				pVertList[vertIndex++] = y+fontSize+translateDown;
				pVertList[vertIndex++] = 0;

				translate+=glyphWidth;
			}
		}

		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(3, GL_FLOAT, 0, num*6,pVertList);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, num*6, pTextureCoordList);

		glDrawArrays(GL_TRIANGLES, 0, count*6);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

#ifndef __PPCGEKKO__
		delete[] pVertList;
		delete[] pTextureCoordList;
#endif

		if(doOrthoProj)
		{
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		}
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);	
		glDisable(GL_BLEND);
	}
}

void GLFontRenderer::setScreenResolution(int screenWidth, int screenHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
}

void GLFontRenderer::setColor(float r, float g, float b, float a)
{
	m_color[0] = r;
	m_color[1] = g;
	m_color[2] = b;
	m_color[3] = a;
}
