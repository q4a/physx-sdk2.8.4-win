#ifndef PIPELINE_STAGES_H
#define PIPELINE_STAGES_H

#include "FluidPPUTypes.h"
#include "PacketHash.h"
#include "DynamicsParameters.h"
#include "FluidParticleArray.h"
#include "ScreenFluidSurfacePara.h"
#include "IndexedTriangleMesh.h"
#include "DensityDepthMap.h"
#include "FluidCollision.h"
#include "FluidParticleUpdateArray.h"

typedef void (*PcmFluidMakePacketHash)(
								PacketHash&						outputPacketHash,		
								FluidParticleArray&				outputParticles,		
								u8vector&						outputDeletedIdsBuffer, 
								u32&							outputDeletedIdsNum,	
								const FluidParticleArray&		inputParticles,	
								const FluidParticleUpdateArray&	inputUpdateStream,
								const DynamicsParameters&		inputParameters);		

typedef void (*PcmFluidPacketPipeline)(
								FluidParticleArray&		        outputParticles,   
								FluidConstraintPair*			outputConstraints,
								const FluidParticleArray&        inputParticles,    
								const FluidConstraintPair*		inputConstraints,
								const PacketHash&		        inputPacketHash,   
								const DynamicsParameters&       inputParameters,   
								const FluidCollision* 			inputCollision,
								u32								mode);   

typedef void (*PcmFluidSimpleParticle)(
								FluidParticleArray&				outputParticles,  
								const FluidParticleArray&		inputParticles,   
								const DynamicsParameters&		inputParameters);

/**************************************************************************************************/
/* For fast fluid surface generation                  
 * by Richard, Greg, Youquan
 * in St.Louis from 8,2005
 */
#include "surfaceConstants.h"

#if FLUID_SURFACE_USE_STANDALONE_ONES

typedef void (*PcmSurfaceCalcDepthDensity)(		
								DensityDepthMap *		        densityDepthMap,    /* Output */
                                u16vector *                     numParticlesPerLine,/* Output */
								const ParticleArrayFW *			particles,		   /* Input */
								ScreenFluidSurfacePara *        parameters);        /* Input */

typedef void (*PcmSurfaceAddVertices)(
								IndexedTriangleMesh *           triangleMesh,       /* Output */
								NodeEdgeVertex32Map *           vertexMap,          /* Output */
								const DensityDepthMap *         densityDepthMap,    /* Input */
                                const u16vector *               numParticlesPerLine,/* Input */
								const ScreenFluidSurfacePara *  fluidSurfacePara);  /* Input */

typedef void (*PcmSurfaceGenerateTriangles)(
								IndexedTriangleMesh *           triangleMesh,       /* Output */
								const NodeEdgeVertex32Map *     vertexMap,          /* Input */
                                const u16vector *               numParticlesPerLine,/* Input */
								const ScreenFluidSurfacePara *  fluidSurfacePara);  /* Input */

typedef void (*PcmSurfaceCalcNormals)(
								IndexedTriangleMesh *           triangleMesh,       /* Output */
								const ScreenFluidSurfacePara *  fluidSurfacePara);  /* Input */

#else

typedef void (*PcmFastFluidSurfaceGen)(
				IndexedTriangleMesh *			triangleMeshHOST,	    /* Output triangle data */
				const ScreenFluidSurfacePara *  parametersHOST,         /* Input control parameters */
				const FluidParticleArray *		particlesHOST);         /* Input particles data */

#endif
//#if FLUID_SURFACE_USE_STANDALONE_ONES

/**************************************************************************************************/
#endif
