
#include <stdio.h>

#include "PerfRenderer.h"
#include "GLFontRenderer.h"
#include "Timing.h"

PerfRenderer::PerfRenderer()
{
	m_profilerOn = false;
	lastTime=getCurrentTime();

	for(unsigned int i=0;i<FPS_AVG_FRAMES;i++)
		fpsTimes[i]=0.0f;
	fpsIndex=0;
}

bool PerfRenderer::toggleEnable()
{
	m_profilerOn = !m_profilerOn;
	return m_profilerOn;
}

static void NextColor(bool reset = false)
{
	static int numColors = 4;
	static float colors[4][3] = {{1.0f,0.0,0.5},{0,1,0},{1,0.5,0.25},{0.5,0.5,0}};
	static int currColor = 0;

	if (reset)
	{
		currColor = 0;
		return;
	}
	GLFontRenderer::setColor(colors[currColor][0],colors[currColor][1],colors[currColor][2],1.0f);

	currColor = (currColor+1) % numColors;
}

void PerfRenderer::render(const NxProfileData* prof, int width, int height)
{
	if(!m_profilerOn)
		return;

	GLFontRenderer::setScreenResolution(width, height);
#ifdef __PPCGEKKO__
	GLFontRenderer::setColor(0.8f, 1.0f, 0.0f, 1.0f);
#else
	GLFontRenderer::setColor(1.0f, 0.0f, 0.0f, 1.0f);
#endif	

	//position on screen:
	float sx = 0.01f;
	float sx2= 0.38f;
	//float col1 = -0.5f/2;
	//float col2 = -0.2f/5;
	float sy = 0.95f;
#ifdef __PPCGEKKO__	
	float fontHeight = 0.036f;
#else
	float fontHeight = 0.018f;
#endif
	char buf[256];
		
	if(prof != NULL)
	{
		// display title
		sprintf(buf,"Profiler (FPS: %g)",computeFPS());
		GLFontRenderer::print(sx, sy, fontHeight, buf, false, 11, true);
		
		//display headings
#ifdef NX_ENABLE_PROFILER_COUNTER
		GLFontRenderer::print(sx2, sy, fontHeight, "count  %parent  %total  %self       hier        self   counter");
#else
		GLFontRenderer::print(sx2, sy, fontHeight, "count  %parent  %total  %self             hier              self");
#endif

		sy -= 2*fontHeight;

		//The total for the top level is the sum of all the top level zones
		//(There could be multiple top level zones, eg threading enabled).
		float total = 0.0f;

		for(unsigned int i=0;i<prof->numZones;i++)
		{
		if(prof->profileZones[i].recursionLevel==0)
			total+=prof->profileZones[i].hierTime;
		}

		NextColor(true);

		for(unsigned int i=0;i<prof->numZones;i++)
		{
			NxProfileZone & z = prof->profileZones[i];
#ifdef __CELLOS_LV2__
			// Skip internal SPU Dynamics profile zones
			//
			if (!strncmp("PXS_PROFILE_ZONE_DYNAMICS_SPU_",z.name,strlen("PXS_PROFILE_ZONE_DYNAMICS_SPU_")))
				continue;
#endif

			if(z.callCount)	//skip empty zones
			{

				float precTotal = total ? z.hierTime * 100.0f / total : 0.0f;
				float percSelf  = z.hierTime ? z.selfTime * 100.0f / z.hierTime : 0.0f;
				
#ifdef NX_ENABLE_PROFILER_COUNTER
				sprintf(buf, "%4d    %4.0f  %4.0f %4.0f %6d %6d %7d", z.callCount, z.percent, precTotal, percSelf, z.hierTime, z.selfTime,z.counter);
#else
				sprintf(buf, "%4d    %4.0f  %4.0f %4.0f %10d %10d", z.callCount, z.percent, precTotal, percSelf, z.hierTime, z.selfTime);
#endif

				GLFontRenderer::print(sx + 0.012f*z.recursionLevel, sy, fontHeight, z.name+2, false, 11, true);

				GLFontRenderer::print(sx2, sy, fontHeight, buf, true, 11, true);

				NextColor();
				
				sy -= fontHeight;
			}
		}
	}
	else if(m_profilerOn)
		{//we can still display frame rate even if NX was not compiled with profiling support.
		// display title
#ifdef __PPCGEKKO__
		sprintf(buf,"FPS: %d",(unsigned int)computeFPS());
#else
		sprintf(buf,"FPS: %g",computeFPS());
#endif
		GLFontRenderer::print(sx, sy, fontHeight, buf, false, 11, true);
		}

}

float PerfRenderer::computeFPS()
{
	// Calculate frame rate
	
	float currTime=getCurrentTime();
	
	fpsTimes[fpsIndex]=currTime-lastTime;
	fpsIndex=(fpsIndex+1)%FPS_AVG_FRAMES;
	
	float avgTime=0.0f;
	for(unsigned int i=0;i<FPS_AVG_FRAMES;i++)
		avgTime+=fpsTimes[i];

	avgTime/=FPS_AVG_FRAMES;
	
	lastTime=currTime;

	return 1.0f/avgTime;
}

