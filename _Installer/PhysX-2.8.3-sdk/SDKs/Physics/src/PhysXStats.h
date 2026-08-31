#ifndef PHYSXSTATS_H

#define PHYSXSTATS_H

#define PHYSX_STATS 0

#if PHYSX_STATS
class NxScene;

void PhysX_Simulate(NxScene *scene,float dtime);
void PhysX_FetchResultsBegin(NxScene *scene);
void PhysX_FetchResultsEnd(NxScene *scene);

#endif


#endif
