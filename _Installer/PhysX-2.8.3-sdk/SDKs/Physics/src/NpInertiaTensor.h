#ifndef __NP_INERTIA_TENSOR__
#define __NP_INERTIA_TENSOR__

NxReal NX_CALL_CONV NpComputeSphereMass			(NxReal radius, NxReal density);
NxReal NX_CALL_CONV NpComputeSphereDensity		(NxReal radius, NxReal mass);
NxReal NX_CALL_CONV NpComputeBoxMass			(const NxVec3& extents, NxReal density);
NxReal NX_CALL_CONV NpComputeBoxDensity			(const NxVec3& extents, NxReal mass);
NxReal NX_CALL_CONV NpComputeEllipsoidMass		(const NxVec3& extents, NxReal density);
NxReal NX_CALL_CONV NpComputeEllipsoidDensity	(const NxVec3& extents, NxReal mass);
NxReal NX_CALL_CONV NpComputeCylinderMass		(NxReal radius, NxReal length, NxReal density);
NxReal NX_CALL_CONV NpComputeCylinderDensity	(NxReal radius, NxReal length, NxReal mass);
NxReal NX_CALL_CONV NpComputeConeMass			(NxReal radius, NxReal length, NxReal density);
NxReal NX_CALL_CONV NpComputeConeDensity		(NxReal radius, NxReal length, NxReal mass);
void NX_CALL_CONV NpComputeBoxInertiaTensor	(NxVec3& diagInertia, NxReal mass, NxReal xlength, NxReal ylength, NxReal zlength);
void NX_CALL_CONV NpComputeSphereInertiaTensor(NxVec3& diagInertia, NxReal mass, NxReal radius, bool hollow);


#endif
