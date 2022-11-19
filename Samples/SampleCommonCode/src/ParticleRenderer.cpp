#include <math.h>
#include "ParticleRenderer.h"
#include "Shaders.h"
#include "NxMath.h"

#include <GL/glut.h>

#define USE_SUBSAMPLE 0         // enable depth subsampling
#define USE_SEPARABLE_BLUR 1

ParticleRenderer::ParticleRenderer() :
    mDisplayMode(PARTICLE_SURFACE),
    mNumParticles(0),
    mPosBuffer(0),
    mPosStride(0),
    mDensityBuffer(0),
    mDensityStride(0),
    mVelocityBuffer(0),
    mVelocityStride(0),
    mWindowW(800),
    mWindowH(600),
    mFov(40.0f),
    mParticleRadius(0.01f),
    mParticleSurfaceScale(1.5f),
    mFbo(0),
    mDepthTex(0),
    mThicknessTex(0),
    mSceneTex(0),
    mSceneDepthTex(0),
    mBlendMode(BLEND_NONE),
    mBlurPasses(1),
    mRenderOffscreen(false),
    mDensityThreshold(500.0f),
    mDensityScale(0.001f),
    mDensityOffset(100.0f),
	//mDensityThreshold(1.0f),
	//mDensityScale(0.001f),
	//mDensityOffset(1.0f),

#if USE_SUBSAMPLE
    mSubSample(2),
#else
    mSubSample(1),
#endif
    mCubemapTex(0)
{
//    glewInit();   // assume this has been done before somewhere

    // load shaders
    mParticleSphereProg = new GLSLProgram(particleVS, particleSpherePS);
    mParticleDensityProg = new GLSLProgram(particleVS, particleDensityPS);
	mParticleFogProg = new GLSLProgram(fogParticleVS, particleFogPS);
//		mParticleFogProg = new GLSLProgram(fogParticleVS, particleThicknessPS);
//    mParticleDebugProg = new GLSLProgram(particleVS, particleDebugPS);

    mParticleSurfaceProg = new GLSLProgram(particleVS, particleSurfacePS);
    mParticleThicknessProg = new GLSLProgram(particleVS, particleThicknessPS);
    mParticleSprayProg = new GLSLProgram(mblurVS, mblurGS, particleSprayPS);
	

#if USE_SEPARABLE_BLUR
    mBlurProg = new GLSLProgram(passThruVS, depthBlurPS);
#else
    mBlurProg = new GLSLProgram(passThruVS, depthBlur2DPS);
#endif
    mDisplaySurfaceProg = new GLSLProgram(passThruVS, displaySurfacePS);
    mDisplaySurfaceChromeProg = new GLSLProgram(passThruVS, displaySurfaceChromePS);
    mDisplaySurfaceNewProg = new GLSLProgram(passThruVS, displaySurfaceNewPS);
    mDisplaySurfaceSolidProg = new GLSLProgram(passThruVS, displaySurfaceSolidPS);

    mTextureRectProg = new GLSLProgram(passThruVS, textureRectPS);
    mDilateProg = new GLSLProgram(passThruVS, dilatePS);

    // default color
    setColor(mColor, 0.0f, 0.5f, 1.0f, 1.0f);

    // original dark blue
    setColor(mFluidColor, 0.5, 0.7, 1.0, 1.0);
    setColor(mFluidColorFalloff, 2.0, 1.0, 0.5, 1.0);
    setColor(mSprayColor, 0.25f, 0.25f, 0.25f, 1.0f);

    mColorTex[0] = 0;
    mColorTex[1] = 0;
    setWindowSize(mWindowW, mWindowH);

    mLightDir[0] = 0.577f;
    mLightDir[1] = 0.577f;
    mLightDir[2] = 0.577f;
}

void ParticleRenderer::setColor(GLfloat *c, float r, float g, float b, float a)
{
    c[0] = r;
    c[1] = g;
    c[2] = b;
    c[3] = a;
}

ParticleRenderer::~ParticleRenderer()
{
    glDeleteTextures(2, mColorTex);
    glDeleteTextures(1, &mDepthTex);
    glDeleteTextures(1, &mSceneTex);
    glDeleteTextures(1, &mSceneDepthTex);

    delete mParticleSphereProg;
//    delete mParticleDebugProg;
    delete mParticleDensityProg;
	delete mParticleFogProg;
    delete mParticleSurfaceProg;
    delete mDisplaySurfaceNewProg;
    delete mDisplaySurfaceChromeProg;
    delete mDisplaySurfaceSolidProg;
    delete mParticleThicknessProg;
    delete mParticleSprayProg;
    delete mBlurProg;
    delete mDisplaySurfaceProg;
    delete mTextureRectProg;
    delete mDilateProg;

    delete mFbo;
}

void ParticleRenderer::setColor(float r, float g, float b, float a)
{
    mColor[0] = r; mColor[1] = g; mColor[2] = b; mColor[3] = a;
} 

void ParticleRenderer::drawPoints()
{
	glVertexPointer(3, GL_FLOAT, mPosStride, mPosBuffer);
	glEnableClientState(GL_VERTEX_ARRAY);

    if (mDensityBuffer) {
        glClientActiveTexture(GL_TEXTURE1);
        glTexCoordPointer(1, GL_FLOAT, mDensityStride, mDensityBuffer);
	    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (mVelocityBuffer) {
        glClientActiveTexture(GL_TEXTURE2);
        glTexCoordPointer(3, GL_FLOAT, mVelocityStride, mVelocityBuffer);
	    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glDrawArrays(GL_POINTS, 0, mNumParticles);

    glDisableClientState(GL_VERTEX_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE0);
}

void ParticleRenderer::drawPointSprites(GLSLProgram *prog, float scale)
{
    // enable blending
    switch(mBlendMode) {
    case BLEND_ALPHA:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
		break;
    case BLEND_ADD:
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        break;
    }

    // enable point sprites
    glEnable(GL_POINT_SPRITE_ARB);
    glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
    glEnable(GL_DEPTH_TEST);

    prog->enable();
   // prog->setUniform1f("pointScale", mWindowH / tan(mFov*0.5*NxPiF64/180.0));
    prog->setUniform1f("pointScale", mSubSampledH / tan(mFov*0.5*NxPiF64/180.0));
    prog->setUniform1f("pointRadius", mParticleRadius*scale);
    prog->setUniform1f("densityThreshold", mDensityThreshold);
    prog->setUniformfv("lightDir", mLightDir, 3);

    // draw points
    drawPoints();

    prog->disable();

    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
    glDisable(GL_POINT_SPRITE_ARB);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void ParticleRenderer::renderSurface()
{
    if (mDisplayMode == PARTICLE_SURFACE || mDisplayMode == PARTICLE_SURFACE_NEW) {
        // render thickness
        mFbo->Bind();
        glViewport(0, 0, mWindowW, mWindowH);
        mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mThicknessTex, GL_COLOR_ATTACHMENT0_EXT);
        mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mSceneDepthTex, GL_DEPTH_ATTACHMENT_EXT);
        mFbo->IsValid();
        glClearColor(0.0, 0.0, 0.0, 0.0); 
        glClear(GL_COLOR_BUFFER_BIT);

        mBlendMode = BLEND_ADD;
        drawPointSprites(mParticleThicknessProg, mParticleSurfaceScale*2.0);
        mBlendMode = BLEND_NONE;
        mFbo->Disable();
    }

    // render eye-space depth to texture
    mFbo->Bind();
    glViewport(0, 0, mWindowW, mWindowH);
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mColorTex[0], GL_COLOR_ATTACHMENT0_EXT);
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mDepthTex, GL_DEPTH_ATTACHMENT_EXT);
    mFbo->IsValid();
    float farEyeZ = -10000.0;
    glClearColor(farEyeZ, 0, 0, 0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawPointSprites(mParticleSurfaceProg, mParticleSurfaceScale);
    mFbo->Disable();

#if USE_SUBSAMPLE
    // subsample depth
    mFbo->Bind();
    glViewport(0, 0, mSubSampledW, mSubSampledH);
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mColorTexLowRes[0], GL_COLOR_ATTACHMENT0_EXT);
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_ATTACHMENT_EXT);
    mFbo->IsValid();
    glClear(GL_COLOR_BUFFER_BIT);

    mTextureRectProg->enable();
    mTextureRectProg->bindTexture("tex", mColorTex[0], GL_TEXTURE_RECTANGLE_ARB, 0);
    glDisable(GL_DEPTH_TEST);
    drawQuad(mWindowW, mWindowH);
    glUseProgram(0);
    mFbo->Disable();
#endif

    switch(mDisplayMode) {
    case PARTICLE_SURFACE:
        displaySurface(mDisplaySurfaceProg);
        break;
    case PARTICLE_SURFACE_CHROME:
        displaySurface(mDisplaySurfaceChromeProg);
        break;
    case PARTICLE_SURFACE_NEW:
        displaySurface(mDisplaySurfaceNewProg);
        break;
    case PARTICLE_SURFACE_SOLID:
        displaySurface(mDisplaySurfaceSolidProg);
        break;
    }
}

void ParticleRenderer::renderSpray()
{
    mBlendMode = BLEND_ADD;
    glColor4fv(mSprayColor);
    drawPointSprites(mParticleSprayProg, 0.5f);
	//drawPointSprites(mParticleSprayProg, 10.5f);
    mBlendMode = BLEND_NONE;
}

void ParticleRenderer::render()
{
    switch(mDisplayMode) {
    case PARTICLE_POINTS:
        glColor4fv(mColor);
        drawPoints();
        break;
 //   case PARTICLE_SPHERES:
	case PARTICLE_DENSITY:
        glColor4fv(mColor);
        drawPointSprites(mParticleSphereProg, 1.0f);
        break;
    //case PARTICLE_DENSITY:
	case PARTICLE_SPHERES:
        mParticleDensityProg->enable();
        mParticleDensityProg->setUniform1f("densityScale", mDensityScale);
        mParticleDensityProg->setUniform1f("densityOffset", mDensityOffset);
        glColor4fv(mColor);
        drawPointSprites(mParticleDensityProg, 1.0f);
        break;
	case PARTICLE_FOG:
		mParticleFogProg->enable();
        mParticleFogProg->setUniform1f("densityScale", 0.012f);
        mParticleFogProg->setUniform1f("densityOffset", 8.5f);
		mDensityThreshold = 5.0f;
		mBlendMode = BLEND_ALPHA;		
		setFogColor(1.0f,1.0f,1.0f, 0.5f);
        glColor4fv(mFogColor);
		//glColor4fv(1.0, 1.0, 1.0, 0.1);
        drawPointSprites(mParticleFogProg, 2.0f);
		
		break;
    case PARTICLE_SURFACE:
    case PARTICLE_SURFACE_NEW:
    case PARTICLE_SURFACE_CHROME:
    case PARTICLE_SURFACE_SOLID:
        renderSurface();
        renderSpray();
        break;
    }

    glutReportErrors();
}

void ParticleRenderer::beginSceneRender()
{
    mFbo->Bind();
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mSceneTex, GL_COLOR_ATTACHMENT0_EXT);
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mSceneDepthTex, GL_DEPTH_ATTACHMENT_EXT);
    mFbo->IsValid();
}

void ParticleRenderer::endSceneRender()
{
    mFbo->Disable();
}

void ParticleRenderer::blurDepth(bool vertical)
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    mFbo->Bind();
#if USE_SUBSAMPLE 
    glViewport(0, 0, mSubSampledW, mSubSampledH);
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mColorTexLowRes[1 - mSrcTex], GL_COLOR_ATTACHMENT0_EXT);
#else
    glViewport(0, 0, mWindowW, mWindowH);
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mColorTex[1 - mSrcTex], GL_COLOR_ATTACHMENT0_EXT);
#endif
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_ATTACHMENT_EXT);

#if USE_SUBSAMPLE 
    mBlurProg->bindTexture("colorTex", mColorTexLowRes[mSrcTex], GL_TEXTURE_RECTANGLE_ARB, 0);
#else
    mBlurProg->bindTexture("colorTex", mColorTex[mSrcTex], GL_TEXTURE_RECTANGLE_ARB, 0);
#endif
    mBlurProg->enable();
    mBlurProg->setUniform2f("scale", 1.0f * !vertical, 1.0f * vertical);
    //mBlurProg->setUniform1f("blurScale", mBlurScale);
    //mBlurProg->setUniform1f("blurDepthFalloff", mBlurDepthFalloff);

#if USE_SUBSAMPLE
    drawQuad(mSubSampledW, mSubSampledH);
#else
    drawQuad(mWindowW, mWindowH);
#endif

    glUseProgram(0);

    mSrcTex = 1 - mSrcTex;

    mFbo->Disable();

    glDepthMask(GL_TRUE);
}

void ParticleRenderer::dilateDepth()
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    mFbo->Bind();
    glViewport(0, 0, mSubSampledW, mSubSampledH);
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mColorTexLowRes[1 - mSrcTex], GL_COLOR_ATTACHMENT0_EXT);
    mFbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_ATTACHMENT_EXT);

    mDilateProg->bindTexture("tex", mColorTexLowRes[mSrcTex], GL_TEXTURE_RECTANGLE_ARB, 0);
    mDilateProg->enable();

    drawQuad(mSubSampledW, mSubSampledH);

    glUseProgram(0);

    mSrcTex = 1 - mSrcTex;

    mFbo->Disable();

    glDepthMask(GL_TRUE);
}

void ParticleRenderer::displaySurface(GLSLProgram *prog)
{
    mSrcTex = 0;

//    dilateDepth();
    for(unsigned int i=0; i<mBlurPasses; i++) {
#if USE_SEPARABLE_BLUR
        blurDepth(true);
        blurDepth(false);
#else
        blurDepth(false);
#endif
    }

    glViewport(0, 0, mWindowW, mWindowH);
    prog->enable();

#if USE_SUBSAMPLE
    prog->bindTexture("colorTex", mColorTexLowRes[mSrcTex], GL_TEXTURE_RECTANGLE_ARB, 1);
#else
    prog->bindTexture("colorTex", mColorTex[mSrcTex], GL_TEXTURE_RECTANGLE_ARB, 1);
#endif
    prog->bindTexture("depthTex", mDepthTex, GL_TEXTURE_RECTANGLE_ARB, 2);
    prog->bindTexture("thicknessTex", mThicknessTex, GL_TEXTURE_RECTANGLE_ARB, 3);
    if (mSceneTex) {
        prog->bindTexture("sceneTex", mSceneTex, GL_TEXTURE_RECTANGLE_ARB, 4);
    }
    if (mCubemapTex) {
        prog->bindTexture("cubemapTex", mCubemapTex, GL_TEXTURE_CUBE_MAP_ARB, 5);
    }

    prog->setUniform2f("invViewport", 1.0f / mWindowW, 1.0f / mWindowH);
    prog->setUniform2f("invFocalLen", mInvFocalLen * mAspect, mInvFocalLen );
//    prog->setUniform1f("depthThreshold", mDepthThreshold);
    prog->setUniformfv("fluidColor", mFluidColor, 4);
    prog->setUniformfv("colorFalloff", mFluidColorFalloff, 4);
    prog->setUniform1f("subSample", mSubSample);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if USE_SUBSAMPLE
    drawQuad(mSubSampledW, mSubSampledH);
#else
    drawQuad(mWindowW, mWindowH);
#endif

    glDisable(GL_BLEND);

    glUseProgram(0);
}

GLuint
ParticleRenderer::createTexture(GLenum target, int w, int h, GLint internalformat, GLenum format)
{
    GLuint texid;
    glGenTextures(1, &texid);
    glBindTexture(target, texid);

    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(target, 0, internalformat, w, h, 0, format, GL_FLOAT, 0);
    return texid;
}

void ParticleRenderer::createBuffers()
{
    GLenum target = GL_TEXTURE_RECTANGLE_ARB;
//    GLenum format = GL_RGBA8;
//    GLenum format = GL_RGBA16F_ARB;
    GLenum format = GL_LUMINANCE32F_ARB;

    mColorTex[0] = createTexture(target, mWindowW, mWindowH, format, GL_LUMINANCE);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    mColorTex[1] = createTexture(target, mWindowW, mWindowH, format, GL_LUMINANCE);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    mDepthTex = createTexture(target, mWindowW, mWindowH, GL_DEPTH_COMPONENT24_ARB, GL_DEPTH_COMPONENT);

#if USE_SUBSAMPLE
    mColorTexLowRes[0] = createTexture(target, mSubSampledW, mSubSampledH, format, GL_LUMINANCE);
    //glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    mColorTexLowRes[1] = createTexture(target, mSubSampledW, mSubSampledH, format, GL_LUMINANCE);
    //glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif

    mThicknessTex = createTexture(target, mWindowW, mWindowH, GL_LUMINANCE16F_ARB, GL_LUMINANCE);
//    mThicknessTex = createTexture(target, mSubSampledW, mSubSampledH, GL_LUMINANCE16F_ARB, GL_LUMINANCE);

    mSceneTex = createTexture(target, mWindowW, mWindowH, GL_RGBA8, GL_RGBA);
    mSceneDepthTex = createTexture(target, mWindowW, mWindowH, GL_DEPTH_COMPONENT24_ARB, GL_DEPTH_COMPONENT);

    if (!mFbo) {
        mFbo = new FramebufferObject();
    }
    mFbo->AttachTexture(target, mColorTex[0], GL_COLOR_ATTACHMENT0_EXT);
    mFbo->AttachTexture(target, mDepthTex, GL_DEPTH_ATTACHMENT_EXT);
    mFbo->IsValid();
}

void ParticleRenderer::setWindowSize(int w, int h)
{
    mWindowW = w;
    mWindowH = h;

    mSubSampledW = mWindowW / mSubSample;
    mSubSampledH = mWindowH / mSubSample;

    mAspect = (float) mWindowW / (float) mWindowH;
    mInvFocalLen = tan(mFov*0.5*NxPiF64/180.0);

    if (mColorTex[0]) {
        glDeleteTextures(2, mColorTex);
        mColorTex[0] = mColorTex[1] = 0;
    }
    if (mDepthTex) {
        glDeleteTextures(1, &mDepthTex);
        mDepthTex = 0;
    }
    if (mThicknessTex) {
        glDeleteTextures(1, &mThicknessTex);
        mThicknessTex = 0;
    }
    if (mSceneTex) {
        glDeleteTextures(1, &mSceneTex);
        mSceneTex = 0;
    }
    if (mSceneDepthTex) {
        glDeleteTextures(1, &mSceneDepthTex);
        mSceneDepthTex = 0;
    }

    createBuffers();
}

void ParticleRenderer::drawQuad(int texW, int texH)
{
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);       glVertex2f(-1.0, -1.0);
    glTexCoord2f(texW, 0);    glVertex2f(1.0, -1.0);
    glTexCoord2f(texW, texH); glVertex2f(1.0, 1.0);
    glTexCoord2f(0, texH);    glVertex2f(-1.0, 1.0);
    glEnd();
}
